#include<memory>
#include<iostream>
#include<systemc>
#include<gmp.h>

#include "rsa.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;


SC_MODULE(RSA256SysC) {
	sc_in_clk clk;
	sc_fifo_in<sc_bv<256>> i_message;
	sc_fifo_in<sc_bv<256>> i_key;
	sc_fifo_in<sc_bv<256>> i_module;
	sc_fifo_out<sc_bv<256>> o_crypto;

	SC_CTOR(RSA256SysC) {
		SC_THREAD(Thread);
	}

	void Thread() {
		sc_bv<256> data;
		mpz_t msg, key, module, crypto;
		mpz_inits(msg, key, module, crypto, NULL);
		char str[256];

		while (true) {
			data = i_message.read();
			mpz_set_str(msg, data.to_string(SC_HEX).c_str(), 0);
			data = i_key.read();
			mpz_set_str(key, data.to_string(SC_HEX).c_str(), 0);
			data = i_module.read();
			mpz_set_str(module, data.to_string(SC_HEX).c_str(), 0);

			rsa(crypto, msg, key, module);

			gmp_snprintf(str, 256, "0x%Zx", crypto);
			o_crypto.write(str);
		}
	}
};

const char str_msg[] =  "0x412820616369726641206874756F53202C48544542415A494C452054524F50";
const char str_key[] = "0x10001";
const char str_module[] = "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_ans[] = "0xD41B183313D306ADCA09126F3FED6CDEC7DCDCE49DB5C85CB2A37F08C0F2E31";
SC_MODULE(Testbench) {

	sc_clock clk;
	sc_fifo<sc_bv<256>> i_message;
	sc_fifo<sc_bv<256>> i_key;
	sc_fifo<sc_bv<256>> i_module;
	sc_fifo<sc_bv<256>> o_crypto;
	RSA256SysC rsa;
	bool timeout, pass;

	vector<uint8_t> in_data, out_data;

	SC_CTOR(Testbench): clk("clk", 1, SC_NS), rsa("rsa"), timeout(true), pass(true) {
		SC_THREAD(Driver);
		SC_THREAD(Monitor);
		rsa.clk(clk);
		rsa.i_message(i_message);
		rsa.i_key(i_key);
		rsa.i_module(i_module);
		rsa.o_crypto(o_crypto);
	}
	void Driver() {
		cout << "Message: " << str_msg << endl;
		cout << "Key: " << str_key << endl;
		cout << "module: " << str_module << endl;
		i_message.write(str_msg);
		i_key.write(str_key);
		i_module.write(str_module);
	}

	void Monitor() {
		sc_bv<256> crypto = o_crypto.read();
		string str_crypto = crypto.to_string(SC_HEX);
		cout << "Crypto message: " << crypto.to_string(SC_HEX) << endl;
		if (crypto == str_ans) {
			cout << "OK" << endl;
		} else {
			cout << "Golden != systemC: " << str_crypto << " vs " << str_ans << endl;
			pass = false;
		}
		timeout = false;
		sc_stop();
	}
};

int sc_main(int, char**)
{
	unique_ptr<Testbench> tb(new Testbench("testbench"));
	sc_start(10, SC_US);
	return 0;
}
