namespace sockets
{
	template<typename T, typename... Types>
	void socket::send(const char *pFmt, T pValue, Types... pRest)
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		const char *c;

		for (c = pFmt; *c;)
		{
			if (*c == '%')
			{
				if (*(++c) != '%')
				{
					mOutBuf << pValue;
					send(c, pRest...);
					return;
				}
				else
				{
					mOutBuf << '%';
				}
			}
			else
			{
				mOutBuf << *c++;
			}
		}
		if (*c == 0)
		{
			logs::warn << LOC() << "Too many arguments given to irc::socket::send("
				"\"" << pFmt << "\", \"" << pValue << "\", ...)" << logs::done;
		}
	}
}
