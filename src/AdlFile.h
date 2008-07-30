#ifndef EASTWOOD_ADLFILE_H
#define EASTWOOD_ADLFILE_H

class CadlPlayer;
class Copl;
class AdlFile
{
	public:
		AdlFile();
		~AdlFile();
		static void callback(void *userdata, Uint8 *audiobuf, int len);
		int m_channels;
		int m_freq;
		Uint16 m_format;
		bool playing;
	
	private:
		Copl *m_opl;
		CadlPlayer *m_adlPlayer;
		unsigned char getsampsize() { 
			return m_channels * (m_format == AUDIO_U8 ? 1 : 2); }

};

#endif // EASTWOOD_ADLFILE_H
