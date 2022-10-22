# not used
# p = 0xED495192F0DFAB70D5FB366B4C9994F3
# q = 0xF2247ECB0567BD368EB463DD0C19D8DB
# phi = 0xE07122F2A4A9E81141ADE518A2CD7572FD48A002B9BE799F013FB8E5B4170614

#number 
# calculate: (c * m * 2**-256) % n 

n = 0xE07122F2A4A9E81141ADE518A2CD7574DCB67060B005E24665EF532E0CCA73E1
e = 0x10001
d = 0x5972DD91C4AC6E6FCA344AD4C9B586B4805B5C6219B2DEF7CEE09D88F680228D

c0 = 0xD41B183313D306ADCA09126F3FED6CDEC7DCDCE49DB5C85CB2A37F08C0F2E31
c1 = 0x2EE3A71DAA16525340F365965046E0D01E348E4619ABFDBC81EA4E4799039ADF
c2 = 0x163090873935f233c78e4f582b1441bb903fff2251d9b7997c772c1fd014ebe9

m0 = 0x412820616369726641206874756F53202C48544542415A494C452054524F50
m1 = 0x63617265646E7520686374754420656874206C6C61632074276E6F442D2950


a0 = 0x13EB7A41A898C7DC918698B71BFDCD781DD2096248C2D219A31D8CB6B375C17D

def pow512():
	k = 1
	if(k > n):
		k -= n
	for i in range(512):
		k <<= 1
		if(k > n):
			k -= n
		#print "k = ", hex(k)
	return k

def montgomery(x,y):
	"""(x * y * 2**-256) % n"""
	"""x = A, y = B"""
	print "X = ",hex(x)
	print "Y = ",hex(y)
	S = 0
	for i in range(128):
		if(x&1):
			S += y
		if(x&2):
			S += y<<1
		print "t_l_k = ", hex(S)

		# make S mod 4 == 0 by add n
		if(S&3 == 0):
			S += (n<<2) 
		if(S&1):
			S += n
		if(S&2):
			S += (n<<1)
		S >>= 2
		if(S>n):
			S -=n
		x >>= 2
		print "T_LAL = ", hex(S)

		assert(1)
	return S

def montgomery_np(x,y):
	"""(x * y * 2**-256) % n"""
	"""x = A, y = B"""
	S = 0
	for i in range(128):
		if(x&1):
			S += y
		if(x&2):
			S += y<<1

		# make S mod 4 == 0 by add n
		if(S&3 == 0):
			S += (n<<2) 
		if(S&1):
			S += n
		if(S&2):
			S += (n<<1)
		S >>= 2
		if(S>n):
			S -=n
		x >>= 2

		assert(1)
	return S

def lsb(x,y):
	"""calculate x**y mod n"""
	"""m = x, e = y"""
	S = 1
	T = x
	#for i in range(256):
	for i in range(256):
		if(y&1):
			S = montgomery(S,T)
		T = montgomery(T,T)
		#if(y&1):
			#print "S = ", hex(S)
		#print "T = ", hex(T)
		y>>=1
	return S

def rsa(x,y):
	"""calculate x**y mod n"""
	"""m = x, e = y"""
	pow2_512 = pow512()
	print "2^512 mod n = ", hex(pow2_512)
	x2 = montgomery_np(x,pow2_512)
	print "a1_MA = ", hex(x2)

	S = 1
	T = x2
	for i in range(1):
		if(y&1):
			S = montgomery_np(S,T)
		T = montgomery(T,T)
		if(y&1):
			print "S = ", hex(S)
		print "T = ", hex(T)
		y>>=1
	return hex(S)

