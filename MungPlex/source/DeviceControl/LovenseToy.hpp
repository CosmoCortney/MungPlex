#pragma once
#include <iostream>
#include "../../Log.hpp"
#include "Lovense.h"
#include "LovenseDef.h"
#include "LovenseToyInfo.h"
#include <map>
#include <memory>
#include "../../Settings.hpp"

namespace MungPlex
{
	class LovenseToy
	{
	private:
		class CEventCallBack: public ILovenseSDKNotify
		{
		public:
			void LovenseDidSearchStart();
			void LovenseSearchingToys(lovense_toy_info_t* info);
			void LovenseErrorOutPut(int errorCode, const char* errorMsg);
			void LovenseDidSearchEnd();
			void LovenseDidSendCmdStart();
			void LovenseSendCmdResult(const char* szToyID, CLovenseToy::CmdType cmd, const char* result, CLovenseToy::Error errorCode);
			void LovenseDidSendCmdEnd();
			void LovenseToyConnectedStatus(const char* szToyID, bool isConnected);
		};

	public:
		LovenseToy();
		void InitManager(const char* token);
		int SendCommand(const CLovenseToy::CmdType cmd, const int param = 0);
		int SearchToy();
		int StopSearchingToy();
		int ConnectToToy();
		int DisconnectToy();
		bool IsInitialized();
		static void SetToyInfo(const lovense_toy_info_t* toyInfo);
		lovense_toy_info_t* GetToyInfo();
		static const char* GetErrorMessageByCode(const int err);

	private:
		std::shared_ptr<CEventCallBack> _callbacks = std::make_shared<CEventCallBack>();
		std::shared_ptr<CLovenseToyManager> _manager;
		std::string _token;
		static inline lovense_toy_info_t _toyInfo;
		int _statusCode = CLovenseToy::TOYERR_UNKNOWN;
		FloorString _statusMessage = FloorString("", 256);
	};
}