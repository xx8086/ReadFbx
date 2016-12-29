#ifndef _FBX_COMMON_H
#define _FBX_COMMON_H

class CCharChange
{
private:
	CCharChange();
	virtual ~CCharChange();
public:
	wchar_t* C2W(char* c, size_t len);
	char* W2C(wchar_t* w, size_t len);
	bool Release();

private:
	bool Init();
	bool Unit();
private:
	char* m_c;
	wchar_t* m_wc;
};



#endif