
typedef unsigned long long uint64_t;


struct inputvalue
{
public:
	int version;
	uint64_t code;
	uint64_t action;
	inline inputvalue()
	{
		version = 0;
		code = 0;
		action = 0;
	}

	void set(uint64_t _code, uint64_t _action);
};