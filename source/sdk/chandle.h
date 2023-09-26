#ifndef SDK_CHANDLE_H_
#define SDK_CHANDLE_H_

/**
 * @brief A safe way to store edicts
*/
class CHandle
{
public:
	CHandle();
	CHandle(edict_t* edict);

	edict_t* Get() const;
	void Set(edict_t* edict);
	void Invalidate();
	bool IsValid() const;

	operator void* ();
	void operator=(edict_t* edict);
	void operator=(CHandle& other);
	bool operator==(edict_t* edict);
	bool operator==(CHandle& other);
	bool operator==(const CHandle& other) const;
	edict_t* operator->();

private:
	edict_t* m_pent; // Pointer to the edict
	int m_serialnumber; // Stored serial number
	int m_index; // Stored index
};

inline edict_t* CHandle::operator->()
{
	return m_pent;
}

inline void CHandle::Invalidate()
{
	m_pent = nullptr;
	m_serialnumber = -1;
	m_index = -1;
}

inline bool CHandle::operator==(CHandle& other)
{
	if (this->m_pent == nullptr)
	{
		return false;
	}
	else if (this->m_serialnumber == other.m_serialnumber && this->m_index == other.m_index)
	{
		return true;
	}

	return false;
}

inline bool CHandle::operator==(const CHandle& other) const
{
	if (this->m_pent == nullptr)
	{
		return false;
	}
	else if (this->m_serialnumber == other.m_serialnumber && this->m_index == other.m_index)
	{
		return true;
	}

	return false;
}

#endif // !SDK_CHANDLE_H_

