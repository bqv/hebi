namespace hydra
{
    template<typename T, typename... Types>
    message::message(command pCmd, const char* pCmd_str, T pValue, Types... pRest)
        : message(pCmd, pCmd_str, pRest...)
    {
        std::ostringstream oss;
        oss << pValue;
        mParams.push_front(oss.str());
    }
}
