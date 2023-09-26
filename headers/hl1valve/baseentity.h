#ifndef SDK_HEADERS_CBASEENTITY_H_
#define SDK_HEADERS_CBASEENTITY_H_

typedef enum { USE_OFF = 0, USE_ON = 1, USE_SET = 2, USE_TOGGLE = 3 } USE_TYPE;

class CBaseEntity
{
public:
	// Constructor.  Set engine to use C/C++ callback functions
	// pointers to engine data
	entvars_t* pev;		// Don't need to save/restore this pointer, the engine resets it

	// path corners
	CBaseEntity* m_pGoalEnt;// path corner we are heading towards
	CBaseEntity* m_pLink;// used for temporary link-list operations. 

	static	TYPEDESCRIPTION m_SaveData[];

	// fundamental callbacks
	void (CBaseEntity ::* m_pfnThink)(void);
	void (CBaseEntity ::* m_pfnTouch)(CBaseEntity* pOther);
	void (CBaseEntity ::* m_pfnUse)(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	void (CBaseEntity ::* m_pfnBlocked)(CBaseEntity* pOther);

	//We use this variables to store each ammo count.
	int ammo_9mm;
	int ammo_357;
	int ammo_bolts;
	int ammo_buckshot;
	int ammo_rockets;
	int ammo_uranium;
	int ammo_hornets;
	int ammo_argrens;
	//Special stuff for grenades and satchels.
	float m_flStartThrow;
	float m_flReleaseThrow;
	int m_chargeReady;
	int m_fInAttack;

	enum EGON_FIRESTATE { FIRE_OFF, FIRE_CHARGE };
	int m_fireState;
};

#endif // !SDK_HEADERS_CBASEENTITY_H_

