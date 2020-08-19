#ifndef utils996
#define utils996
#include <string>
#include <algorithm>
#include <fstream>
#include <cctype>
#include <locale.h>
#include <functional>
#include <vector>
#include <Windows.h>//

std::wstring s2ws(const std::string& str);
std::string  ws2s(const std::wstring &str);
std::string to_lower(std::string str);

class Base64
{
public:
	/*编码
	DataByte
	[in]输入的数据长度,以字节为单位
	*/
	std::string Encode(char* Data, int DataByte);
	/*解码
	DataByte
	[in]输入的数据长度,以字节为单位
	OutByte
	[out]输出的数据长度,以字节为单位,请不要通过返回值计算
	输出数据的长度
	*/
	std::string Decode(const char* Data, int DataByte, int& OutByte);
};

class MD5
{
public:
	typedef unsigned int size_type; // must be 32bit  

	MD5();
	MD5(const std::string& text);
	void update(const unsigned char *buf, size_type length);
	void update(const char *buf, size_type length);
	MD5& finalize();
	std::string hexdigest() const;
	friend std::ostream& operator<<(std::ostream&, MD5 md5);

private:
	void init();
	typedef unsigned char uint1; //  8bit  
	typedef unsigned int uint4;  // 32bit  
	enum { blocksize = 64 }; // VC6 won't eat a const static int here  

	void transform(const uint1 block[blocksize]);
	static void decode(uint4 output[], const uint1 input[], size_type len);
	static void encode(uint1 output[], const uint4 input[], size_type len);

	bool finalized;
	uint1 buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk  
	uint4 count[2];   // 64bit counter for number of bits (lo, hi)  
	uint4 state[4];   // digest so far  
	uint1 digest[16]; // the result  

	// low level logic operations  
	static inline uint4 F(uint4 x, uint4 y, uint4 z);
	static inline uint4 G(uint4 x, uint4 y, uint4 z);
	static inline uint4 H(uint4 x, uint4 y, uint4 z);
	static inline uint4 I(uint4 x, uint4 y, uint4 z);
	static inline uint4 rotate_left(uint4 x, int n);
	static inline void FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
};

std::string md5(const std::string str);
std::vector<std::string> SplitString(const std::string& s, const std::string& c);

static  std::string ltrim(std::string s);
static  std::string rtrim(std::string s);
std::string s_trim(std::string s);
#endif