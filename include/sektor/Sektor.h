#ifndef __SC_SEKTOR_
#define __SC_SEKTOR_

struct SektorID
{
    SektorID(u64 id) : id(id) {}
    SektorID(s16 _x, s16 _y, s16 _z): x(_x), y(_y), z(_z), count(0) {}
    
    bool operator() (const SektorID& x, const SektorID& y) const {return x.id<y.id;}
    operator u64() {return id;}
    
    union
    {
        struct
        {
            // x, y z beschreiben sektorposition im grid
            // count wird hochgezählt, falls mehr als ein sektor sich an der selben Grid-Position befindet
            s16 x;
            s16 y;
            s16 z;
            s16 count;
        };
        u64 id;
    };
};

enum SektorType
{
    SEKTOR_NONE = 0,
    SEKTOR_ROOT = 1,
    SUPER_GALAXIE_CLUSTER = 2,    
    SUPER_GALAXIE = 3,
    GALAXIE_CLUSTER = 4,
    GALAXIE = 5,
    STAR_CLUSTER = 6,
    SOLAR_SYSTEM = 7,
    STELLAR_BODY = 8,
    PLANET = 9,
    SUB_PLANET = 10
};

/*!
 * \brief ein Sektor ist ein Teil der Welt
 * 
 * class Sektor
 * ein Sektor ist ein Teil der Welt, mit einer bestimmten Größe.. ein bestimmter Zoomausschnitt
 * ein Sektor kann wiederum aus vielen unter-Sektoren bestehen
 * bei Sektoren wie Galaxien haben die Unter-Sektoren beliebige ids
 * bei Sektoren mit hoher Materie-Dichte (wie PLaneten, Planetenteile) beschreibt die SektorID auch die Position
 * im Grid, in dem sich alle Sektoren der gleichen Ebene befinden
 * Sektoren sind hierachisch aufgebaut
 * => : enthält 
 * 1 => 2, 2 => 3, 3 => 4, 4 => 6 usw.
 * contains other sektors 
 * can be a stellar body (like a planet, etc), a star-system, a galaxie,
 * a star-heap (sternenhaufen), a galaxie-heap,
 * a super-galaxie-heap, a super-galaxie 
 */

class Sektor
{
public:
    Sektor(Vector3Unit position, Unit radius, SektorID id, Sektor* parent);
    virtual ~Sektor();
    
    /*! \brief render sektor and childs
     * 
     *  using transformation and render sektor,     *  
     */    
    virtual DRReturn render(float fTime, Camera* cam) = 0;
    
    /*! \brief call render for all parents and childs
     *
     * first traverse to root node and then call the childs rekursiv
     * \param fTime time since last frame for render purpose
     * \param cam pointer to camera for correct positioning and culling
     * \param rootRendered set to false (used intern)
     */
    DRReturn renderAll(float fTime, Camera* cam, bool rootRendered = false);
    /*! \brief move sektor
     * 
     * check if child are visible, and move (logical, physical)
     * sektors longer not visible, will be deleted
     * generate all sektors, which are now visible,
     * 
     */
    virtual DRReturn move(float fTime, Camera* cam) = 0;
    
    /*! \brief call move for all parents and childs
     *
     * first traverse to root node and then call the childs rekursiv
     * \param fTime time since last frame for move purpose
     * \param cam pointer to camera for correct positioning and culling
     * \param rootMoved set to false (used intern)
     */
    DRReturn moveAll(float fTime, Camera* cam, bool rootMoved = false);
    
    static const char* getSektorTypeName(SektorType type);
    
    //! inline getter and setter
    __inline__ RenderSektor* getRenderer() {return mRenderer;}
    __inline__ Vector3Unit getPosition() {return mSektorPosition;}
    __inline__ Unit getRadius() {return mRadius;}
    
protected:
    virtual bool isPlayerInIt(Camera* cam) {return false;}
    
    //! id des sektors, gleichzeitig der seed
    SektorID            mID;
    SektorType          mType;
    //! Position of sektor inside the parent
    Vector3Unit         mSektorPosition; 
    //! die größe des Sektors, oder der Sektor-Einschließenden Kugel
    Unit                mRadius;
    //! Pointer at the parent-sektor
    Sektor*             mParent;
    
    //! renderer for this sektor
    RenderSektor*       mRenderer;
    
    std::map<u64, Sektor*> mChilds;
    typedef std::pair<u64, Sektor*> SEKTOR_ENTRY;
    
private:
    
    DRReturn renderChilds();
    DRReturn renderParents();
    DRReturn moveChilds();
    DRReturn moveParents();
};

#endif //__SC_SEKTOR_