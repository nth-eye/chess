
#pragma once

namespace chess {

inline std::string sq2str(int sq)
{
	return std::string(1, 'a' + sq%8) + std::string(1, '1' + sq/8);
}

inline constexpr uint8_t rf2sq(const int r, const int f) 
{ 
	return r*8 + f; 
}

inline constexpr bool sq_valid(const int r, const int f) 
{ 
	return (unsigned)(r | f) < 8; 
}

inline constexpr bool sq_valid(const unsigned int sq) 
{ 
	return sq < 64; 
}

}