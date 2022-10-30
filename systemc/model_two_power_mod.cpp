#include <memory>
#include <iostream>
#include <systemc>
#include <gmp.h>

#include "rsa.h"
#include "verilog_int.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace verilog;

constexpr int kBW = 256;
typedef vint<false, 256> KeyType;

SC_MODULE(RSATwoPowerMod)
{
	sc_in_clk clk;
	sc_fifo_in<sc_uint<32>> i_power;
	sc_fifo_in<KeyType> i_modulus;
	sc_fifo_out<KeyType> o_out;

	SC_CTOR(RSATwoPowerMod)
	{
		SC_THREAD(Thread);
	}

	void Thread()
	{
		mpz_t modulus, out;
		mpz_inits(modulus, out);
		char str[256];

		while (true)
		{
			int power = i_power.read();
			KeyType data = i_modulus.read();
			mpz_set_str(modulus, to_hex(data).c_str(), 16);

			two_power_mod(out, power, modulus);

			gmp_snprintf(str, 256, "0x%ZX", out);
			from_hex(data, str);
			o_out.write(data);
		}
	}
};

const char str_modulus[] = "0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1";
const char str_ans[] = "AF39E1F831CB4FCD92B17F61F473735C687593A931C97D2B60AD6C7443F09FDB";
const int power = 512;
SC_MODULE(Testbench)
{

	sc_clock clk;
	sc_fifo<sc_uint<32>> i_power;
	sc_fifo<KeyType> i_modulus;
	sc_fifo<KeyType> o_out;
	RSATwoPowerMod two_power_mod;
	bool timeout, pass;

	SC_CTOR(Testbench) : clk("clk", 1, SC_NS), two_power_mod("two_power_mod"), timeout(true), pass(true)
	{
		SC_THREAD(Driver);
		SC_THREAD(Monitor);
		two_power_mod.clk(clk);
		two_power_mod.i_power(i_power);
		two_power_mod.i_modulus(i_modulus);
		two_power_mod.o_out(o_out);
	}

	void Driver()
	{
		sc_uint<32> power = 512;
		KeyType modulus;
		cout << "calculate 2^: " << power << endl;
		cout << "modulus: " << str_modulus << endl;
		from_hex(modulus, str_modulus);
		i_power.write(power);
		i_modulus.write(modulus);
	}

	void Monitor()
	{
		KeyType out = o_out.read();
		string str_out = to_hex(out);
		cout << "Output: " << str_out << endl;
		if (str_out == str_ans)
		{
			cout << "OK" << endl;
		}
		else
		{
			cout << "Golden != systemC: " << str_out << " vs " << str_ans << endl;
			pass = false;
		}
		timeout = false;
		sc_stop();
	}
};

int sc_main(int, char **)
{
	unique_ptr<Testbench> tb(new Testbench("testbench"));
	sc_start(10, SC_US);
	return 0;
}
