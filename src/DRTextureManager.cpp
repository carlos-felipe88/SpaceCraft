#include "main.h"

DRTextureManager::DRTextureManager()
: mInitalized(false), mTextureLoadMutex(NULL), mTextureLoadThread(NULL), mTextureLoadCondition(NULL),
  mTextureLoadSemaphore(NULL)
{
	mTextureLoadSemaphore = SDL_CreateSemaphore(1);
	mTextureLoadCondition = SDL_CreateCond();
	SDL_SemWait(mTextureLoadSemaphore);
	mTextureLoadMutex = SDL_CreateMutex();   
#if SDL_VERSION_ATLEAST(1,3,0)
	mTextureLoadThread = SDL_CreateThread(asynchronTextureLoadThread, "DRTexLoa", this);
#else
	mTextureLoadThread = SDL_CreateThread(asynchronTextureLoadThread, this);
#endif
}

DRTextureManager& DRTextureManager::Instance()
{
    static DRTextureManager TheOneAndOnly;
    return TheOneAndOnly;
}

DRReturn DRTextureManager::init()
{
    mInitalized = true;
    LOG_INFO("DRTextureManager initalisiert");
    return DR_OK;
}

void DRTextureManager::exit()
{
    mInitalized = false;
    for(std::multimap<DHASH,TextureMemoryEntry>::iterator it = mTextureMemoryEntrys.begin(); it != mTextureMemoryEntrys.end(); it++)
    {
        glDeleteTextures(1, &it->second.textureID);
    }
    mTextureMemoryEntrys.clear();
    
    for (int i = 0; i < mTextureEntrys.getNItems(); i++)
	{
		TextureEntry* texture = static_cast<TextureEntry*>(mTextureEntrys.findByIndex(i));
        DR_SAVE_DELETE(texture->texture);
        DR_SAVE_DELETE(texture);
		DRGrafikError("Fehler beim Texturen löschen in: DRTextureManager::Exit");
	}
	mTextureEntrys.clear(true);    

	if(mTextureLoadThread)
	{
		//Post Exit to Stream Thread
		SDL_SemPost(mTextureLoadSemaphore); LOG_WARNING_SDL();
		//kill TextureLoad Thread after 1/2 second
		SDL_Delay(500);
		SDL_KillThread(mTextureLoadThread);
		LOG_WARNING_SDL();

		mTextureLoadThread = NULL;
		SDL_DestroySemaphore(mTextureLoadSemaphore);
		SDL_DestroyMutex(mTextureLoadMutex);
		SDL_DestroyCond(mTextureLoadCondition);
	}
    
    LOG_INFO("DRTextureManager beendet");
}

DHASH DRTextureManager::makeTextureHash(const char* filename, GLint glMinFilter/* = GL_LINEAR*/, GLint glMagFilter /*= GL_LINEAR*/)
{
    DHASH hash = DRMakeFilenameHash(filename);
    hash |=  glMinFilter| glMagFilter << 4;
    return hash;
}
DHASH DRTextureManager::makeTextureHash(const TextureMemoryEntry &entry)
{
    return entry.width | entry.height<<4 | entry.bpp << 8 | entry.format << 16;    
}

//! lädt oder return instance auf Textur
DRTextur* DRTextureManager::getTexture(const char* filename, GLint glMinFilter/* = GL_LINEAR*/, GLint glMagFilter/* = GL_LINEAR*/)
{
	if(!mInitalized) return 0;

    DHASH id = makeTextureHash(filename, glMinFilter, glMagFilter);
	//Schauen ob schon vorhanden
	TextureEntry* entry = static_cast<TextureEntry*>(mTextureEntrys.findByHash(id));
	
	if(entry && entry->texture != NULL)
	{
		entry->referenzCounter++;
		return entry->texture;
	}

	entry = new TextureEntry;
	entry->referenzCounter = 0;
	
    entry->texture = new DRTextur(filename, glMinFilter, glMagFilter);
	if(!entry->texture)
	{
		DR_SAVE_DELETE(entry);
        LOG_ERROR("No Memory for new Texture left", 0);
	}
    
    if(!mTextureEntrys.addByHash(id, entry))
	{
        DR_SAVE_DELETE(entry->texture);
		DR_SAVE_DELETE(entry);
		LOG_ERROR("Unerwarteter Fehler in DRTextureManager::getTexture aufgetreten", 0);
	}
	entry->referenzCounter = 1;

	return entry->texture;
}
//! reduziert reference, bei null wird Textur gelöscht und OpenGL Texture in liste eingetragen
void DRTextureManager::releaseTexture(const char* filename, GLint glMinFilter/* = GL_LINEAR*/, GLint glMagFilter/* = GL_LINEAR*/)
{
    if(!mInitalized) return;
    
    //suchen
    DHASH id = makeTextureHash(filename, glMinFilter, glMagFilter);
	TextureEntry* entry = static_cast<TextureEntry*>(mTextureEntrys.findByHash(id));
	if(!entry) return;

	entry->referenzCounter--;
	if(entry->referenzCounter <= 0)
	{
        freeTexture(entry->texture->removeTexturID());
        DR_SAVE_DELETE(entry->texture);
		
		mTextureEntrys.removeByHash(id);
		DR_SAVE_DELETE(entry);
	}
}
//! schaut nach ob solche eine Texture in der Liste steckt, wenn nicht wird eine neue erstellt
GLuint DRTextureManager::getGLTextureMemory(GLuint width, GLuint height, GLuint bpp, GLuint format)
{
    if(!mInitalized) return 0;
    
    TextureMemoryEntry entry(width, height, bpp, format);
    DHASH id = makeTextureHash(entry);
    if(mTextureMemoryEntrys.find(id) != mTextureMemoryEntrys.end())
    {
        entry = mTextureMemoryEntrys.find(id)->second;
        mTextureMemoryEntrys.erase(id);
        return entry.textureID;
    }
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
    GLuint format2 = GL_RGB;
    if(format == 4) format2 = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
		format2, bpp, 0);
    
    return textureID;
}
//! packt die Textur in die Liste, falls noch jemand den Speicher benötigt
void DRTextureManager::freeTexture(GLuint textureID)
{
    if(!mInitalized) return;
    
    TextureMemoryEntry entry;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &entry.width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &entry.height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, &entry.bpp);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &entry.format);
    //printf("Infos ueber eine zu befreiende Texture: ");
    //entry.print(); printf("\n");
    entry.textureID = textureID;
    entry.timeout = 600.0f;
    DHASH id = makeTextureHash(entry);
    mTextureMemoryEntrys.insert(DR_TEXTURE_MEMORY_ENTRY(id, entry));       
}
    
// update timeout, release lange nicht verwendete Texturen
DRReturn DRTextureManager::move(float fTime)
{
    if(!mInitalized) return DR_ERROR;
        
    for(std::multimap<DHASH, TextureMemoryEntry>::iterator it = mTextureMemoryEntrys.begin(); it != mTextureMemoryEntrys.end(); it++)
    {
        it->second.timeout -= fTime;
        if(it->second.timeout < 0.0f)
        {
            printf("DRTextureManager::move, timeout texture will be deleted\n");
            glDeleteTextures(1, &it->second.textureID);
            mTextureMemoryEntrys.erase(it);
        }
    }
	SDL_LockMutex(mTextureLoadMutex);
	if(!mLoadedAsynchronLoadTextures.empty())
	{
		mLoadedAsynchronLoadTextures.front()->pixelsCopyToRenderer();
		mLoadedAsynchronLoadTextures.pop();
	//	printf("\rtextures to load left: %d", mAsynchronLoadTextures.size());
	}
	SDL_UnlockMutex(mTextureLoadMutex);
    return DR_OK;
}

void DRTextureManager::addAsynchronTextureLoadTask(Texture* textur)
{
	SDL_LockMutex(mTextureLoadMutex); LOG_WARNING_SDL();
	mAsynchronLoadTextures.push(textur);

	//send the texture load Thread a signal to continue
	if(SDL_CondSignal(mTextureLoadCondition)== -1) //LOG_ERROR_SDL(DR_ERROR);
	{
		LOG_WARNING_SDL();
		LOG_WARNING("Fehler beim Aufruf von SDL_CondSignal"); 
	}

	SDL_UnlockMutex(mTextureLoadMutex); LOG_WARNING_SDL();
}

int DRTextureManager::asynchronTextureLoadThread(void* data)
{
	printf("asynchronTextureLoadThread start\n");
	DRTextureManager& t = DRTextureManager::Instance();
	while(SDL_SemTryWait(t.mTextureLoadSemaphore)==SDL_MUTEX_TIMEDOUT)
	{
		// Lock work mutex
		SDL_LockMutex(t.mTextureLoadMutex); LOG_ERROR_SDL(-1);
		int status = SDL_CondWait(t.mTextureLoadCondition, t.mTextureLoadMutex); LOG_ERROR_SDL(-1);
		if( status == 0)
		{
			while(!t.mAsynchronLoadTextures.empty())
			{
				// get top textur
				Texture* cur = t.mAsynchronLoadTextures.front();
				t.mAsynchronLoadTextures.pop();
				SDL_UnlockMutex(t.mTextureLoadMutex);
				// call load
				cur->loadFromFile();
				SDL_LockMutex(t.mTextureLoadMutex);
				// push it onto the other queue
				t.mLoadedAsynchronLoadTextures.push(cur);
				//SDL_UnlockMutex(t.mTextureLoadMutex);

			}
			SDL_UnlockMutex(t.mTextureLoadMutex); LOG_ERROR_SDL(-1);
		}
		else
		{
			//unlock mutex and exit
			SDL_UnlockMutex(t.mTextureLoadMutex); LOG_ERROR_SDL(-1);
			LOG_ERROR("Fehler im Stream Thread, exit", -1);
		}
	}

	return 0;
}

void DRTextureManager::test()
{
    printf("\n------ DRTextureManager::test ------\n");
    TextureMemoryEntry first(800, 600, 32, 4);
    TextureMemoryEntry second(800, 600, 32, 3);
    TextureMemoryEntry third(1280, 1024, 32, 3);
    TextureMemoryEntry four(1280, 1024, 32, 4);
    printf("first: "); first.print(); printf(" HASH: %d\n", makeTextureHash(first));
    printf("second: "); second.print(); printf(" HASH: %d\n", makeTextureHash(second));
    printf("third: "); third.print(); printf(" HASH: %d\n", makeTextureHash(third));
    printf("four: "); four.print(); printf(" HASH: %d\n", makeTextureHash(four));
        
    printf("\n--- DRTextureManager::test ende ----\n");
}