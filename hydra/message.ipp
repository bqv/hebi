namespace hydra
{
    template<typename T, typename... Types>
    message::message(command pCmd, const char* pCmd_str, T pValue, Types... pRest)
    {
        std::ostringstream oss;
        oss << pValue;
        mParams.push_back(oss.str());
        message(pCmd, pCmd_str, pRest...);
    }
}
