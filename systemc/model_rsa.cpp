#include <memory>
#include <iostream>
#include <systemc>
#include "verilog_int.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

constexpr int kBW = 256;
typedef vint<false, 256> KeyType;

SC_MODULE(RSA256SysC) {
	sc_in_clk clk;
	sc_fifo_in<KeyType> i_message;
	sc_fifo_in<KeyType> i_key;
	sc_fifo_in<KeyType> i_module;
	sc_fifo_out<KeyType> o_crypto;

	SC_CTOR(RSA256SysC) {
		SC_THREAD(Thread);
	}

	void Thread() {
		while (true) {
			KeyType message = i_message.read();
			KeyType key = i_key.read();
			KeyType modular = i_module.read();
			KeyType crypto;

			o_crypto.write(crypto);
		}
	}
};

const char str_msg[] =  "412820616369726641206874756F53202C48544542415A494C452054524F50";
const char str_key[] = "10001";
const char str_module[] = "E07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_ans[] = "D41B183313D306ADCA09126F3FED6CDEC7DCDCE49DB5C85CB2A37F08C0F2E31";
SC_MODULE(Testbench) {
	sc_clock clk;
	sc_fifo<KeyType> i_message;
	sc_fifo<KeyType> i_key;
	sc_fifo<KeyType> i_module;
	sc_fifo<KeyType> o_crypto;
	RSA256SysC rsa;
	bool timeout, pass;

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
		KeyType message, key, modular;
		from_hex(message, str_msg);
		from_hex(key, str_key);
		from_hex(modular, str_module);
		i_message.write(message);
		i_key.write(key);
		i_module.write(modular);
	}

	void Monitor() {
		string gotten = to_hex(o_crypto.read());
		if (gotten == str_ans) {
			cout << "OK" << endl;
		} else {
			cout << "Golden != systemC: " << gotten << " vs " << str_ans << endl;
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
