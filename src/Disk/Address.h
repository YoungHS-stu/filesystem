#pragma once
#include"utils.h"
class Address
{
public:
    unsigned char addr[3]; // 3*8=24 bit address, unsigned 保证高位不扩展, 方便转int
	~Address() {}
    Address() {
        memset(addr, 0, sizeof(addr));
    }
    Address(int addrInt) {
        from_int(addrInt);
    }
    void from_int(int addrInt) {
        addr[0] = addrInt % 256;
		addrInt /= 256;
		addr[1] = addrInt % 256;
		addrInt /= 256;
		addr[2] = addrInt % 256;
    }
    int to_int() const {
        return (int)addr[0] * 1 + (int)addr[1] * 256 + (int)addr[2] * 256 * 256;
    }
    // !计算对应的block的地址
    Address block_addr() {
		return Address((to_int() / 1024) << 10);
	}
	Address offset() {
		return Address(to_int() % 1024);
	}
    Address operator+(const Address& a) {
		return Address(this->to_int() + a.to_int());
	}
	Address operator+(const int& a) {
		return Address(this->to_int() + a);
	}
	Address operator-(const Address& a) {
		return Address(this->to_int() - a.to_int());
	}
	Address operator-(const int& a) {
		return Address(this->to_int() - a);
	}
	bool operator==(const Address& a) {
		return this->to_int() == a.to_int();
	}
	bool operator==(const int& a) {
		return this->to_int() == a;
	}
	bool operator!=(const Address& a) {
		return !(*this == a);
	}
	bool operator!=(const int& a) {
		return !(*this == a);
	}
};