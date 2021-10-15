class Address
{
public:
    unsigned char addr[3]; // 3*8=24 bit address, unsigned 保证高位不扩展, 方便转int
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
    // 计算所在block的地址
    Address block_addr() {
		return Address((to_int() / 1024) << 10);
	}
	Address offset() {
		return Address(to_int() % 1024);
	}

}