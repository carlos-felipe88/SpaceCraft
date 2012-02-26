#include "main.h"
#include "ShaderManager.h"
#include "DRGeometrieManager.h"

struct ControlMode
{
    ControlMode() {}
    ControlMode(Unit value) : mValue(value) {}
    ControlMode(double value, UnitTypes type) : mValue(value, type) {}
    Unit mValue;
};

// Globale Variablen
Player g_Player;
RenderBlockLoader g_RenderBlockLoader;
Camera* g_cam = NULL;
DRFont* g_Font = NULL;
DRTextur* g_tex = NULL;
DRTextur* g_terrain = NULL;
int blockCount = 100;
#define MAX_CONTROL_MODES 9
ControlMode gControlModes[MAX_CONTROL_MODES];
int gCurrentControlMode = 0;
// Debug
bool            wireframe = false;
//DRGeometrieIcoSphere g_geo;

GLint sphereList = 0;



int rekursionTest(int zahl)
{
    int z[20];
    if(zahl > 10000) return zahl;
    return rekursionTest(++zahl);
}

void test()
{
    //tests
    DRMatrix m1(DRMatrix::identity());
    float mat[] = {1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};
    if(memcmp(m1, mat, sizeof(float)*16) != 0)
        LOG_WARNING("matrix identity isn't valid");
    DRMatrix m2 = m1.rotationX(30.0f);
    DRMatrix m3 = DRMatrix::axis(DRVector3(1.0f, 0.0f, 0.0f),
                                 DRVector3(0.0f, 1.0f, 0.0f),
                                 DRVector3(0.0f, 0.0f, 1.0));
    if(memcmp(m1, m3, sizeof(float)*16) != 0)
        LOG_WARNING("matrix axis isn't valid");
    
    DRLog.writeMatrixToLog(m1);
    DRLog.writeMatrixToLog(m2);
    DRLog.writeMatrixToLog(m3);
    
    DRLog.writeToLog("RekursionTest: %d", rekursionTest(0));
    
    
    //Speicher test
/*  LOG_INFO("Speichertest");
    std::list<void*> pointer;
    void* t = NULL;
    u32 count = 0;
    do
    {
        t = malloc(16384);
        if(t) pointer.push_back(t);
        count++;
        if(count > 192073)
            break;
    } while(t);
    
    DRLog.writeToLog("count: %d, %u kByte wurden reserviert!", count, count*16384/1024);
    
    for(std::list<void*>::iterator it = pointer.begin(); it != pointer.end(); it++)
        free(*it);
    pointer.clear();
   //* */
    
    // Unit test
    printf("\n");
    Unit parsec(1.0, PARSEC);
    Unit lj = parsec.convertTo(LIGHTYEAR);
    DRLog.writeToLog("%s -> %s", parsec.print().data(), lj.print().data());    
    lj = Unit(1.0, LIGHTYEAR);
    parsec = lj.convertTo(PARSEC);
    DRLog.writeToLog("%s -> %s", lj.print().data(), parsec.print().data());    
    Unit ae = lj.convertTo(AE);
    DRLog.writeToLog("%s -> %s", lj.print().data(), ae.print().data());    
    ae = parsec.convertTo(AE);
    DRLog.writeToLog("%s -> %s", parsec.print().data(), ae.print().data());    
    parsec = ae.convertTo(PARSEC);
    DRLog.writeToLog("%s -> %s", ae.print().data(), parsec.print().data());    
    Unit m = parsec.convertTo(M);
    DRLog.writeToLog("%s -> %s", parsec.print().data(), m.print().data());    
    Unit kpc(1.0, KILOPARSEC);
    m = kpc.convertTo(M);
    DRLog.writeToLog("%s -> %s", kpc.print().data(), m.print().data());    
    m = Unit(1.0, M);
    kpc = m.convertTo(KILOPARSEC);
    DRLog.writeToLog("%s -> %s", m.print().data(), kpc.print().data());    
    printf("\n");
    
    Unit aes(0.005, AE);
    DRLog.writeToLog("%s -> %s", aes.print().data(), aes.convertTo(KM).print().data());
    
    //Vector Unit Test
    Vector3Unit u1(100, 200, 70, M), u2(1, 0, 0, KILOPARSEC), u3(100, 20, 17, LIGHTYEAR);
    u1.print("u1");
    u2.print("u2");
    u3.print("u3");
    
    u1 *= Unit(20, KM);
    u1.print("u1* 20 km");
    
    Vector3Unit(u1 + u2).print("u1+u2");
    Vector3Unit(u2+u3).print("u2+u3");
    Vector3Unit(u1*Unit(1, LIGHTYEAR)).print("u1*1 Lichtjahr");
    
    DRVector3 v(1.0f, 7.0f, 2.0f);
    DRLog.writeVector3ToLog(v, "init");
    v = v.normalize();
    DRLog.writeVector3ToLog(v, "normalized");
    v *= 7.0f;
    DRLog.writeVector3ToLog(v, "multiplikator");
    
    // ----------------------------------  ReferenzHolder Test --------------------------------
    
    DRLog.writeToLog("DRIndexReferenzHolder test");
    DRIndexReferenzHolder referenzHolder(10);
    uint tests[10];
    tests[0] = referenzHolder.getFree();
    referenzHolder.add(tests[0]);
    tests[1] = referenzHolder.getFree();
    
    DRLog.writeToLog("index1 (0): %d, index2 (1): %d", tests[0], tests[1]);
    referenzHolder.remove(tests[0]);
    tests[2] = referenzHolder.getFree();
    referenzHolder.remove(tests[1]);
    tests[3] = referenzHolder.getFree();
    DRLog.writeToLog("index3 (2): %d, index4 (1): %d", tests[2], tests[3]);
    for(int i = 0; i < 5; i++)
        tests[4+i] = referenzHolder.getFree();
    referenzHolder.remove(tests[7]);
    tests[9] = referenzHolder.getFree();
    DRLog.writeToLog("index10: (6): %d", tests[9]);
    
    DRTextureManager::Instance().test();
}

void sizeOfClasses()
{
    DRLog.writeToLog("--------  Klassen-Objekt groessen (in Bytes): -----------");
	DRLog.writeToLog("Textur: %d", sizeof(Texture));
    DRLog.writeToLog("Camera: %d", sizeof(Camera));
    DRLog.writeToLog("DRGeometrieIcoSphere: %d", sizeof(DRGeometrieIcoSphere));
    DRLog.writeToLog("DRGeometrieHeightfield: %d", sizeof(DRGeometrieHeightfield));
	DRLog.writeToLog("DRMatrix: %d", sizeof(DRMatrix));
	DRLog.writeToLog("DRTextureManager: %d", sizeof(DRTextureManager));
    DRLog.writeToLog("DRVector3: %d", sizeof(DRVector3));
    DRLog.writeToLog("GenerateNoisePlanet: %d", sizeof(GenerateNoisePlanet));
    DRLog.writeToLog("PlanetHeightValues: %d", sizeof(PlanetHeightValues));
    DRLog.writeToLog("PlanetSektor: %d", sizeof(PlanetSektor));
    DRLog.writeToLog("RenderSektor: %d", sizeof(RenderSektor));
    DRLog.writeToLog("RenderPlanet: %d", sizeof(RenderPlanet));
    DRLog.writeToLog("RenderSubPlanet: %d", sizeof(RenderSubPlanet));
    DRLog.writeToLog("RenderInStepsToTexture: %d", sizeof(RenderInStepsToTexture));
    DRLog.writeToLog("RenderNoisePlanetToTexture: %d", sizeof(RenderNoisePlanetToTexture));
    DRLog.writeToLog("Sektor: %d", sizeof(Sektor));
    DRLog.writeToLog("SubPlanetSektor: %d", sizeof(SubPlanetSektor));
    DRLog.writeToLog("Unit: %d", sizeof(Unit));
    DRLog.writeToLog("Vector3Unit: %d", sizeof(Vector3Unit));
    DRLog.writeToLog("------- Klassen-Objekt groessen Ende ----------");
}

DRReturn generateSphere(DRReal radius);
DRReturn load()
{
    if(EnInit_Simple())
        return DR_ERROR;
    DRFileManager::Instance().addOrdner("data/blockView");
    DRFileManager::Instance().addOrdner("data/shader");
    test();
    sizeOfClasses();
        
    DRRandom r;
    srand(77111);
    
    //Steuerung
  
    gControlModes[0].mValue = Unit(20, M);
    gControlModes[1].mValue = Unit(0.100, KM);
    gControlModes[2].mValue = Unit(10, KM);
    gControlModes[3].mValue = Unit(1000, KM);
    gControlModes[4].mValue = Unit(20000, KM);
    gControlModes[5].mValue = Unit(400000, KM);
    gControlModes[6].mValue = Unit(0.1, AE);
    gControlModes[7].mValue = Unit(10, AE);
    gControlModes[8].mValue = Unit(500, AE);
     
    //if(EnInit_OpenGL(1.0f, DRVideoConfig(800, 600), "Space Craft - Techdemo"))
    if(EnInit_INI("data/config.ini"))
        LOG_ERROR("Fehler bei init OpenGL", DR_ERROR);       
	LOG_WARNING_SDL();

    
    g_Font = new DRFont();
    g_Font->init("data/MalgunGothic.tga", "data/MalgunGothic.tbf");

    DRLog.writeToLog("CPU-Count: %d", g_CPU_Count);
    
  //  glShadeModel(GL_SMOOTH);
  //  glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
   // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

    //Reseten der Matrixen
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(40.0f, (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.1f, 2000.0f);
    glMatrixMode(GL_MODELVIEW);          // Select the modelview matrix

    glLoadIdentity();                    // Reset (init) the modelview matrix


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_SMOOTH);             // Enable smooth shading
    glClearDepth(1.0f);                  // Clear depth buffer
    glEnable(GL_DEPTH_TEST);             // Enables depth test
    glDepthFunc(GL_LEQUAL);              // Type of depth test to perform

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glLineWidth(1.0f);
    glEnable(GL_NORMALIZE);

    // Enhances image quality
    glColor3f(0.5f, 0.5f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    //glEnable(GL_LIGHTING);
    glDisable(GL_FOG);
    
	if(GlobalRenderer::getSingleton().init("data/config.ini"))
		LOG_ERROR("error by init GlobalRenderer", DR_ERROR);
    
    if(DRTextureManager::getSingleton().init())
        LOG_ERROR("error by init DRTextureManager", DR_ERROR);
    if(ShaderManager::getSingleton().init())
        LOG_ERROR("error by init ShaderManager", DR_ERROR);
    if(DRGeometrieManager::getSingleton().init())
        LOG_ERROR("error by init DRGeometrieManager", DR_ERROR);

    if(g_Player.init())
        LOG_ERROR("Fehler bei Player::init", DR_ERROR);
    g_cam = g_Player.getCamera();
       
    if(g_RenderBlockLoader.init())
        LOG_ERROR("Fehler bei RenderBloockLoader::init", DR_ERROR);
    
    Uint32 start = SDL_GetTicks();
    generateSphere(2.0f);
    //g_terrain = new DRTextur("data/terrainsurface.bmp", GL_NEAREST, GL_NEAREST);
    DRLog.writeToLog("%.0f Sekunden fuer Planeten laden/generieren", ((float)SDL_GetTicks()-start)/1000.0f);

    return DR_OK;
}

void ende()
{
    DR_SAVE_DELETE(g_tex);
    glDeleteLists(sphereList, 1);
    g_Player.exit();
    DR_SAVE_DELETE(g_Font);
    DR_SAVE_DELETE(g_terrain);
    GlobalRenderer::getSingleton().exit();
    DRTextureManager::getSingleton().exit();
    ShaderManager::getSingleton().exit();
    DRGeometrieManager::getSingleton().exit();
    g_RenderBlockLoader.exit();
    Server::freeAllServer();
    EnExit();
}

DRReturn move(float fTime)
{
    float fRotSpeed = 2.0f;
    float fSpeed = 20.0f;
	int numKeys = 0;
    //Kamera
#if SDL_VERSION_ATLEAST(1,3,0)
	Uint8 *keystate = SDL_GetKeyboardState(&numKeys);
#else
	Uint8 *keystate = SDL_GetKeyState(NULL);
#endif
	
	/*for(uint i = 0; i < numKeys; i++)
	{
		if(keystate[i]) DRLog.writeToLog("%d button pressed: %d", i, (int)keystate[i]);
	}
	//*/
//	return DR_OK;
    
    int mouseMove_x = 0, mouseMove_y = 0;
    // holen der Maus bewegung seit letztem frame und der bitmaks welche Tasten gedrückt sind
    Uint8 mousePressed = SDL_GetRelativeMouseState(&mouseMove_x, &mouseMove_y);

    //if(gCurrentControlMode != 0 )
    //{
         // die Kamera wird rotiert, gesteuert durch die Tasten w, s (x Achse, hoch/runter), <-, -> (y Achse links/rechts), e und q (z Achse seitlich)
        g_cam->rotateRel(DRVector3(keystate[SDLK_s]-keystate[SDLK_w], keystate[SDLK_RIGHT]-keystate[SDLK_LEFT], keystate[SDLK_q]-keystate[SDLK_e])*fTime);
        // wenn die rechte maustaste gedrückt ist
        if((mousePressed & 4) == 4)
            // wird die Kamera auch abhängig von der Mausposition gedreht
        g_cam->rotateRel(DRVector3(-mouseMove_y, -mouseMove_x, 0.0f)*fTime*fRotSpeed);
    //}

    if(gControlModes[gCurrentControlMode].mValue.getType() == M)
        g_cam->translateRel(DRVector3(keystate[SDLK_d]-keystate[SDLK_a], keystate[SDLK_PAGEUP]-keystate[SDLK_PAGEDOWN], keystate[SDLK_DOWN]-keystate[SDLK_UP])*fTime*gControlModes[gCurrentControlMode].mValue);
    else
        g_cam->translateRel_SektorPosition(DRVector3(keystate[SDLK_d]-keystate[SDLK_a], keystate[SDLK_PAGEUP]-keystate[SDLK_PAGEDOWN], keystate[SDLK_DOWN]-keystate[SDLK_UP])*fTime*gControlModes[gCurrentControlMode].mValue, gControlModes[gCurrentControlMode].mValue.getType());    
    
    //set control mode
    if(EnIsButtonPressed(SDLK_1)) gCurrentControlMode = 0;
    else if(EnIsButtonPressed(SDLK_2)) gCurrentControlMode = 1;
    else if(EnIsButtonPressed(SDLK_3)) gCurrentControlMode = 2;
    else if(EnIsButtonPressed(SDLK_4)) gCurrentControlMode = 3;
    else if(EnIsButtonPressed(SDLK_5)) gCurrentControlMode = 4;
    else if(EnIsButtonPressed(SDLK_6)) gCurrentControlMode = 5;
    else if(EnIsButtonPressed(SDLK_7)) gCurrentControlMode = 6;
    else if(EnIsButtonPressed(SDLK_8)) gCurrentControlMode = 7;
    else if(EnIsButtonPressed(SDLK_9)) gCurrentControlMode = 8;
     
    // R-Taste
    if(EnIsButtonPressed(SDLK_r)) wireframe = !wireframe;
    
    //if(EnIsButtonPressed(SDLK_z)) blockCount++;
    if(keystate[SDLK_z]) blockCount++;
    
    if(fTime == 0.0f) fTime = 0.00166f;

	DRTextureManager::Instance().move(fTime);
    //if(g_Player.getSektor()->moveAll(fTime, g_cam))
    if(g_Player.getSektor()->moveAll(fTime, g_Player.getCamera()))
        LOG_ERROR("Fehler bei move sektor", DR_ERROR);

    return DR_OK;
}

#define TWOPI (PI*2.0f)
#define PID2  (PI/2.0f)
void CreateSphere(DRVector3 c,double r,int n)
{
	int i,j;
	double theta1,theta2,theta3;
	DRVector3 e,p;

	if (r < 0)
		r = -r;
	if (n < 0)
		n = -n;
	if (n < 4 || r <= 0) {
		glBegin(GL_POINTS);
		glVertex3f(c.x,c.y,c.z);
		glEnd();
		return;
	}

	for (j=0;j<n/2;j++) {
		theta1 = j * TWOPI / n - PID2;
		theta2 = (j + 1) * TWOPI / n - PID2;

		glBegin(GL_QUAD_STRIP);
		for (i=0;i<=n;i++) {
			theta3 = i * TWOPI / n;

			e.x = cos(theta2) * cos(theta3);
			e.y = sin(theta2);
			e.z = cos(theta2) * sin(theta3);
			p.x = c.x + r * e.x;
			p.y = c.y + r * e.y;
			p.z = c.z + r * e.z;

			glNormal3f(e.x,e.y,e.z);
			glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
			glVertex3f(p.x,p.y,p.z);

			e.x = cos(theta1) * cos(theta3);
			e.y = sin(theta1);
			e.z = cos(theta1) * sin(theta3);
			p.x = c.x + r * e.x;
			p.y = c.y + r * e.y;
			p.z = c.z + r * e.z;

			glNormal3f(e.x,e.y,e.z);
			glTexCoord2f(i/(double)n,2*j/(double)n);
			glVertex3f(p.x,p.y,p.z);
		}
		glEnd();
	}
}

const float tao = 1.61803399;
DRReturn generateSphere(DRReal radius)
{
    float percent = 1.0f;
    const int iterator = 100;
    
    int totalSegments = 400;
    int currentSegments = (int)((float)totalSegments*percent);
        
    int vertexCount = currentSegments*currentSegments;
    int indexCount =  2*currentSegments*currentSegments-2*currentSegments; //2*currentSegments*currentSegments;//2*currentSegments-1+2*currentSegments*currentSegments;
//    const int iSegments = 200;
//    const int segs = 200;
    printf("vertexCount: %d, indexCount: %d, currentSegments: %d\n", vertexCount, indexCount, currentSegments);
    
    //DRGeometrieIcoSphere geo2;        
   DRGeometrieIcoSphere geo;        
   // DRGeometrieSphere geo;    
    geo.initIcoSphere(5);
    for(int i = 0; i < 4; i++)
        geo.changeGeometrieTo(3, true);
    //geo.initSphere(totalSegments);
    //geo.makeSphericalLandscape(iterator, 7157);
    //geo.copyDataToVertexBuffer();
    
    
    //if(geo.initSphere(totalSegments))
      //  LOG_ERROR("Fehler bei SphereInit", DR_ERROR);
    vertexCount = geo.getVertexCount();
    currentSegments = sqrtf(vertexCount);
    printf("currentSegments: %d ", currentSegments);
    
    DRVector3* points = geo.getVertexPointer();// new DRVector3[vertexCount];
    DRColor* color = geo.getColorPointer();// new DRColor[vertexCount];
    DRColor* heighMap = new DRColor[vertexCount];
    GLuint*  indices = geo.getIndexPointer();        
    
    //*/
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDisable(GL_CULL_FACE);
    
    //glBegin(GL_LINE_LOOP);
  
    
    const char* path = "data/planet.png";
  
    DRIImage* image = DRIImage::newImage();
    DRReturn ret = image->loadFromFile(path);
    float vektorLenght = 1.0f;
    if(!ret && image->getWidth()*image->getHeight() == vertexCount)
    {
        image->getPixel(color);
        int size = image->getWidth()*image->getHeight();
        printf("size: %u\n", size);        
    }
    else
    {
        //geo.makeSphericalLandscape(iterator, 9312);
        GenerateNoisePlanet* g = new GenerateNoisePlanet(); //GlobalRenderer::Instance().getGenerateNoisePlanet();
        g->setupGenerator(182172);
        
        float max = 0.0f, min = 1.0f;
        for(int i = 0; i < vertexCount; i++)
        {
            points[i] *= 1.0f+g->getValue(points[i])*0.01f;
            float l = vektorLenght - points[i].length();
            if(l == vektorLenght) continue;
           // printf("%d: length: %f\n",i, points[i].length());
            if(l > max) max = l;
            if(l < min) min = l;
        }
        //einfärben
        DRLog.writeToLog("min: %f, max: %f", min, max);
        for(int j = 0; j < vertexCount; j++)
        {
        //    DRLog.writeToLog("\n---------- j:%d -------------", j);
            float d = vektorLenght - points[j].length();
            //d = g->getValue(points[j]);
            heighMap[j] = DRColor((DRReal)(fabs((d+min))/(max+fabs(min))));
			color[j] = heighMap[j];
			continue;

    //        if(j < 10)
      //          DRLog.writeToLog("heighMapValue: %f, d: %f", fabs((d+min))/(max+fabs(min)), d);
            d *= -1.0f;
            //if(j < 10)
          //      DRLog.writeToLog("d: %f", d);
            if(d < 0)
            {
                if(d == -1.0f)
                    color[j] = color[(int)fabs((float)(vertexCount-currentSegments-j))].interpolate(color[j-currentSegments], 0.5f);
                else
                    color[j] = DRColor(0.0f, (1.0f-(d/min))/10.0f, 1.0f-(d/min));
            }
            else if(d > 0)
            {
                if(d > max/2)
                    color[j] = DRColor((d/max), (d/max)/2.0f, 0.1f);
                else if(d < max-max/10)
                    color[j] = DRColor((d/max)/2.0f, (d/max), 0.1f);
                else
                    color[j] = DRColor(d/max);
            }
            //DRLog.writeVector3ToLog(points[j]);
            //DRLog.writeColorToLog(color[j]);
        }
        //DRLog.writeToLog("\n-------- ende -------------");
        printf("%d Pixel in ein %d großes Bild\n", vertexCount, currentSegments * (currentSegments+2));
        image->setWidth(currentSegments);
        image->setHeight(currentSegments);
        image->setImageFormat(-1);
        image->setPixel(color);
        image->saveIntoFile(path);    
        
        image->setPixel(heighMap);
        image->saveIntoFile("data/heighmap.png");

		DR_SAVE_DELETE(g);
		
    } 
    
    g_tex = new DRTextur(image);
   // */
	geo.copyDataToVertexBuffer();
	geo.update();
    sphereList = glGenLists(1);
    glNewList(sphereList, GL_COMPILE);     
    
    glPushMatrix();          
    glScalef(radius, radius, radius);
    glColor3f(1.0f, 0.0f, 0.0f);
    geo.render();
    
    glPopMatrix();
    
    glEndList();
        
 //   DR_SAVE_DELETE_ARRAY(points);
//    DR_SAVE_DELETE_ARRAY(color);
//    DR_SAVE_DELETE_ARRAY(indices);
    DR_SAVE_DELETE_ARRAY(heighMap);
    
//    g_tex = new DRTextur(path, GL_NEAREST, GL_NEAREST);
   
    
    return DR_OK;
}

DRReturn render(float fTime)
{
    glViewport(0, 0, g_pSDLWindow->w, g_pSDLWindow->h);
    
    glClearColor(0.1, 0.2, 0.0, 0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);    
    
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    //if(g_terrain)
      //  g_terrain->bind();
    //if(g_Player.getSektor()->renderAll(fTime, g_Player.getCamera()))
    if(wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    if(g_Player.getSektor()->renderAll(fTime, g_Player.getCamera()))
        LOG_ERROR("Fehler bei render sektor", DR_ERROR);
    ShaderProgram::unbind();
    glDisable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    
    glClear (GL_DEPTH_BUFFER_BIT);
     
    
    //Reseten der Matrixen
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //gluPerspective(g_Player.getCameraFOV(), (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.1f, 2000.0f);
    glMultMatrixf(DRMatrix::view_frustum(g_Player.getCameraFOV(), (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.1f, 2000.0f));
    DRFrustumCulling cull(g_cam, g_Player.getCameraFOV(), (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);          // Select the modelview matrix

    glLoadIdentity();                    // Reset (init) the modelview matrix
    DRVector3 translate(0.0f);
    g_cam->setKameraMatrix();
    glEnable(GL_DEPTH_TEST);             // Enables depth test
        
    //light
    //Add ambient light
    GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f}; //Color(0.2, 0.2, 0.2)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    
    //Add positioned light
    GLfloat lightColor0[] = {1.0f, 1.0f, 1.0f, 1.0f}; //Color (0.5, 0.5, 0.5)
    GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glDisable(GL_LIGHT0);
    
    //Add directed light
    GLfloat lightColor1[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glEnable(GL_LIGHT1);       
    
    if(g_tex)
        g_tex->bind();
      
    //glColor3f(0.2f, 0.5f, 0.1f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(1.0, 0.0f);
        glVertex3f( 50.0f, 0.0f, -50.0f);
        glTexCoord2f(0.0, 0.0f);
        glVertex3f(-50.0f, 0.0f, -50.0f);
        glTexCoord2f(0.0, 1.0f);
        glVertex3f(-50.0f, 0.0f,  50.0f);
        glTexCoord2f(1.0, 1.0f);
        glVertex3f( 50.0f, 0.0f,  50.0f);
    glEnd();
    
    //glDisable(GL_TEXTURE_2D);
    
    glTranslatef(0.0f, 2.0f, 0.0f);
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();
    glTranslatef(0.0f, -2.0f, 0.0f);
    
  //  printf("bevore renderBlock\n");
    
    glTranslatef(0.0f, 10.0f, 0.0f);
    translate.y += 10.0f;
    RenderBlock* rb =  g_RenderBlockLoader.getRenderBlock("dirt");
    rb->render();
    
    glTranslatef(0.0f, -5.0f, 0.0f);
    translate.y -= 5.0f;
    rb = g_RenderBlockLoader.getRenderBlock("dirG");
    rb->render();
    glTranslatef(1.0f, 0.0f, 0.0f);
    translate.x += 1.0f;
    rb->render();
    glTranslatef(0.0f, 2.0f, 0.0f);
    translate.y += 2.0f;
    
	DRFrustumPosition res = cull.isBoxInFrustum(DRVector3(-0.5f), DRVector3(0.5f), DRMatrix::translation(translate));
        if(res != OUTSIDE)    
                g_RenderBlockLoader.getRenderBlock("benc")->render();
    
    glDisable(GL_TEXTURE_2D);
    //glDisable(GL_LIGHTING);
    glPushMatrix();
    
    glTranslatef(0.0, 0.0f, -15.0f);
    //renderSphere(5.0f);
    
    static float sphereRotate = 0;
    glRotatef(sphereRotate, 0.0f, 1.0f, 0.0f);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //GlobalRenderer::Instance().getPlanetShaderPtr()->bind();
    if(sphereList)
        glCallList(sphereList);
    glTranslatef(5.0f, 0.0f, 0.0f);
    CreateSphere(DRVector3(), 1.0f, 20.0f);
    //GlobalRenderer::Instance().getPlanetShaderPtr()->unbind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //g_geo.render();
    
    //sphereRotate += fTime*10.0f;
      
    
    glPopMatrix();
    //*/
    static u32 start = 0;
    float dir[] = {1.0f, 1.0f};
    int y = 0;
    const int length = 250;
    int clipCount = 0;
    int renderCount = 0;
    
    for(int i = 0; i < blockCount; i++)
    {           
        if(!(i % 10))
        {
            glTranslatef(0.0f, 1.0f, 0.0f);
            translate.y += 1.0f;
            
            if(cull.isSphereInFrustum(translate, 0.6f) != OUTSIDE)
            {
                rb->render();
                renderCount++;
            }
            else clipCount++;
            
            glTranslatef(0.0f, -1.0f, 0.0f);
            translate.y -= 1.0f;
        }
        if(!(i % length))
        {
            if(!(y % length))
            {
                glTranslatef(0.0f, 1.0f, 0.0f);
                translate.y += 1.0f;
                dir[1] *= -1.0f;
                dir[0] *= -1.0f;
            }
            else
            {
                glTranslatef(0.0f, 0.0f, 1.0f*dir[1]);
                translate.z += 1.0f*dir[1];
                dir[0] *= -1.0f;
             
            }
            y++;
        }
        else
        {
            glTranslatef(1.0f*dir[0], 0.0f, 0.0f);
            translate.x += 1.0f*dir[0];
        }
        if(cull.isSphereInFrustum(translate, 0.6f) != OUTSIDE)
        {
                rb->render();
                renderCount++;
        }
        else clipCount++;
    }
    u32 end = SDL_GetTicks();
          
    //FPS
    g_Font->begin();
        
    DRText text(g_Font);
    text.setFlags(DR_FF_RELATIVE | DR_FF_RELATIVSCALING);
    text.setText("FPS: %.0f", 1.0f/fTime);
    text.setPosition(DRVector2(0.0f, 0.0f));
    text.setColor12(DRColor(0.8f, 0.5f, 0.1f));
    text.setScaling(DRVector2(1.0f));
    text.drawText();    
    
    text.setText("Count: %d", blockCount);
    text.setPosition(DRVector2(0.0f, 0.04f));
    text.drawText();
    
    text.setText("milliseconds: %u", end-start);
    text.setPosition(DRVector2(0.1f, 0.0f));
    text.drawText();
    
    text.setText("Steuerung: %d - %s/s", gCurrentControlMode+1, gControlModes[gCurrentControlMode].mValue.print().data());
    /*switch(controlMode)
    {
        case 0: text.setText("Steuerung: (NONE)"); break;
        case 1: text.setText("Steuerung: 1 - 20 m/s"); break;
        case 2: text.setText("Steuerung: 3 - 20.000 km/s"); break;
        case 3: text.setText("Steuerung: 4 - 0.005 AE/s"); break;
        case 4: text.setText("Steuerung: 2 - 100 km/s"); break;
        default: text.setText("Steuerung: (default)");
    }
     */
    text.setPosition(DRVector2(0.0f, 0.08f));
    text.drawText();
    
    g_Font->end();
   
    start = SDL_GetTicks();    
    if(GlobalRenderer::Instance().renderTasks())
        LOG_ERROR("Fehler bei calling GlobalRenderer::renderTasks", DR_ERROR);

    return DR_OK;
}

#ifdef _WIN32
#undef main
#endif
int main(int argc, char* argv[])
{
    
    printf("Bitte warten, waehrend SpaceCraft geladen wird\n");
    if(load())
    {
        printf("Es trat ein Fehler bei load auf, das Programm wird beendet!\n");
        return -1;
    }

    if(EnGameLoop(move, render))
    {
        printf("Fehler in der GameLoop\n");
        return -1;
    }
   // * */
    ende();
    printf("Auf wiedersehen ;)\n");
    return 42;
}
