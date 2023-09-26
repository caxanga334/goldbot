#ifndef MOD_HL1_H_
#define MOD_HL1_H_

class CHL1Mod : public CBaseMod
{
public:
	CHL1Mod();
	virtual ~CHL1Mod();

	virtual const char* GetModName() override;
	virtual const char* GetModDataDirectory() override;

private:

};

#endif // !MOD_HL1_H_

