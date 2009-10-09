#if defined(__GNUC__)
	#ifndef GNUC_ATTRIBUTE
		#define GNUC_ATTRIBUTE(attr) __attribute__(attr)
	#endif
#else
	#ifndef GNUC_ATTRIBUTE
		#define GNUC_ATTRIBUTE(attr)
	#endif
	#pragma pack(1)
#endif
