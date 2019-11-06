#include<string>
#include<vector>

#include "../cqp/cqp.h"
#include "curlpost.h"
#include "./MJson.h"

#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����

using namespace std;

int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;
vector<int64_t> uid;
vector<char *> saveInput;


/*
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/*
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/* ����ת������������� */
string static strToggle(const string & input)
{
	string output("");
	for (size_t i = 0; i < input.length();)
	{
		if (i + 4 < input.length())
		{
			string sub = input.substr(i, 5);
			if (sub == "&amp;")
			{
				output.append("&");
				i += 5;
				continue;
			}
			else if (sub == "&#91;") {
				output.append("[");
				i += 5;
				continue;
			}
			else if (sub == "&#93;") {
				output.append("]");
				i += 5;
				continue;
			}
			else if (sub == "&#44;") {
				output.append(",");
				i += 5;
				continue;
			}
		}
		if (input[i] == '&') {
			output.append("&amp;");
			++i;
		}
		else if (input[i] == '[') {
			output.append("&#91;");
			++i;
		}
		else if (input[i] == ']') {
			output.append("&#93;");
			++i;
		}
		else if (input[i] == ']') {
			output.append("&#44;");
			++i;
		}
		else {
			output+=input[i];
			++i;
		}
	}
	return output;
}
int Between(char* pcBuf, char* pcRes)
{
	char* pcBegin = NULL;
	char* pcEnd = NULL;

	pcBegin = strstr(pcBuf, "input");
	pcEnd = strstr(pcBuf, "inputend");

	if (pcBegin == NULL || pcEnd == NULL || pcBegin > pcEnd)
	{
		printf("Mail name not found!\n");
	}
	else
	{
		pcBegin += strlen(":");
		memcpy(pcRes, pcBegin, pcEnd - pcBegin);
	}

	return 0;
}

/* ���ڲ����Ļ�ȡ */
static vector<string> getarg(const string & str) {
	int flag = 0;
	vector<string> vec(3);
	for (size_t i = 0; i < str.length(); ++i) {
		if (isspace(static_cast<unsigned char>(str[i])) || isblank(static_cast<unsigned char>(str[i]))) {
			if (flag == 0) {
				if (vec[0] == "")continue;
				++flag; continue;
			}
			else if (flag == 1) {
				if (vec[1] == "")continue;
				++flag; continue;
			}
			else {
				if (vec[2] == "")continue;
			}
		}
		vec[flag] += str[i];
	}
	return vec;
}

string runall(const char* lan, const char* content,const char* input) noexcept
{
	//MessageBoxA(NULL, content, "", 0);
	static vector<pair<string, string>> lanmap
	{
		{"assembly","main.asm"},
		{"ats","main.dats"},
		{"bash","main.sh"},
		{"c","main.c"},
		{"clojure","main.clj"},
		{"cobol","main.cob"},
		{"coffeescript","main.coffee"},
		{"cpp","main.cpp"},
		{"crystal","main.cr"},
		{"csharp","main.cs"},
		{"d","main.d"},
		{"elixir","main.ex"},
		{"elm","Main.elm"},
		{"erlang","main.erl"},
		{"fsharp","main.fs"},
		{"go","main.go"},
		{"groovy","main.groovy"},
		{"haskell","main.hs"},
		{"idris","main.idr"},
		{"java","Main.java"},
		{"javascript","main.js"},
		{"julia","main.jl"},
		{"kotlin","main.kt"},
		{"lua","main.lua"},
		{"mercury","main.m"},
		{"nim","main.nim"},
		{"ocaml","main.ml"},
		{"perl","main.pl"},
		{"perl6","main.pl6"},
		{"php","main.php"},
		{"plaintext","main.txt"},
		{"python","main.py"},
		{"ruby","main.rb"},
		{"rust","main.rs"},
		{"scala","main.scala"},
		{"swift","main.swift"},
		{"typescript","main.ts"}
	};
	auto split = [](string src, const string& substr) ->vector<string> {
		vector<string> retvec;
		size_t pos;
		while (true)
		{
			if ((pos = src.find_first_of(substr)) == string::npos) {
				retvec.push_back(src);
				break;
			}
			else {
				retvec.push_back(src.substr(0, pos));
				src = src.substr(pos + 1);
			}
		}
		return retvec;
	};
	for (const pair<string, string>& i : lanmap) {
		string filename = i.second;
		string website = "https://glot.io/run/" + i.first + "?version=latest";
		string ret;
		//split(filename, ".").at(1)
		if (split(filename, ".").at(1) == lan) {
			try {
				Json::Value  J;
				J["files"][0]["name"] = filename;
				J["files"][0]["content"] = Http::GBKToUTF8(content);
				if (input != NULL) {
					J["stdin"] = strToggle(input);
				}
				
				Json::FastWriter fastWriter;
				string postdata = fastWriter.write(J);
				ret = Http(website, { { "User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.100 Safari/537.36" } }).Post(postdata);
				return Http::UTF8ToGBK(MJson(ret)["stdout"].asString() + MJson(ret)["error"].asString() + MJson(ret)["stderr"].asString());
			} 
			catch (std::exception& e) {
				return   Http::UTF8ToGBK(ret)+ e.what();
			}
		}
	}
	for (const pair<string, string>& i : lanmap) {
		if (lan == i.first) {
			return "Please reply:\ncode " + split(i.second, ".").at(1) + "\nYourCode";
		}
	}
	return "No this language";
}

char* GetInput(char* msg) {
	char* m = strstr(msg, "input");
	if (m != NULL) {
		int length = strlen(msg);
		char* a = new char[length - 4];
		memcpy(a, msg + 5, length - 4);
		return a;
	}
	return NULL;
}
void Add(int64_t qq, char* msg) {
	//CQ_sendGroupMsg(ac, 750462328, msg); 
	for (int i = 0; i < uid.size(); i++) {
		if (uid[i] == qq) {
			saveInput[i] = msg;
			return;
		}
	}
	uid.push_back(qq);
	saveInput.push_back(msg);
}

char* GetSaveInput(int64_t qq) {
	for (int i = 0; i < uid.size(); i++) {
		if (uid[i] == qq) {
			return saveInput[i];
		}
	}
	return NULL;
}
/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {
	vector<string> vec = getarg(msg);
	//��ȡinput
	char* tmpMsg = new char[strlen(msg) + 1];
	strcpy(tmpMsg, msg);
	char* input = GetInput(tmpMsg);
	if (input != NULL) {
		Add(fromQQ, input);
		char* reply = new char[strlen("�ѱ�������\n") + strlen(input)];
		*reply = '\0';
		strcat(reply, "�ѱ�������\n");

		strcat(reply, input);
		CQ_sendPrivateMsg(ac, fromQQ, reply);
	}


	if (vec[0] == "code")
	{
		//����ת��
		vec[2] = strToggle(vec[2]);

		//��ȡ���н��
		string ret = runall(vec[1].c_str(), vec[2].c_str(), GetSaveInput(fromQQ));

		//����ת��
		ret = strToggle(ret);

		//����AT
		ret = "[CQ:at,qq=" + to_string(fromQQ) + "]\n" + ret;

		if (ret.length() >= 3000)
		{
			ret = "��Ӧ�����������ضϴ���\n" + ret;
			ret = ret.substr(0, 3000);
		}
		CQ_sendPrivateMsg(ac, fromQQ, ret.c_str());
	}
	return EVENT_IGNORE;
}


/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	vector<string> vec = getarg(msg);
	//��ȡinput
	char* tmpMsg = new char[strlen(msg) + 1];
	strcpy(tmpMsg, msg);
	char* input = GetInput(tmpMsg);
	if (input != NULL) {
		Add(fromQQ, input);
		char* reply = new char[strlen("�ѱ�������\n") + strlen(input)];
		*reply = '\0';
		strcat(reply, "�ѱ�������\n");
		strcat(reply, input);
		CQ_sendGroupMsg(ac, fromGroup, reply);
	}
	
	
	if (vec[0] == "code")
	{
		//����ת��
		vec[2] = strToggle(vec[2]);

		//��ȡ���н��
		string ret = runall(vec[1].c_str(), vec[2].c_str(), GetSaveInput(fromQQ));

		//����ת��
		ret = strToggle(ret);

		//����AT
		ret = "[CQ:at,qq=" + to_string(fromQQ) + "]\n" + ret;

		if (ret.length() >= 3000)
		{
			ret = "��Ӧ�����������ضϴ���\n" + ret;
			ret = ret.substr(0, 3000);
		}
		CQ_sendGroupMsg(ac, fromGroup, ret.c_str());
	}
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=4 ��������Ϣ
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	vector<string> vec = getarg(msg);
	//��ȡinput
	char* tmpMsg = new char[strlen(msg) + 1];
	strcpy(tmpMsg, msg);
	char* input = GetInput(tmpMsg);
	if (input != NULL) {
		Add(fromQQ, input);
		char* reply = new char[strlen("�ѱ�������\n") + strlen(input)];
		*reply = '\0';
		strcat(reply, "�ѱ�������\n");
		strcat(reply, input);
		CQ_sendDiscussMsg(ac, fromDiscuss, reply);
	}


	if (vec[0] == "code")
	{
		//����ת��
		vec[2] = strToggle(vec[2]);

		//��ȡ���н��
		string ret = runall(vec[1].c_str(), vec[2].c_str(), GetSaveInput(fromQQ));

		//����ת��
		ret = strToggle(ret);

		//����AT
		ret = "[CQ:at,qq=" + to_string(fromQQ) + "]\n" + ret;

		if (ret.length() >= 3000)
		{
			ret = "��Ӧ�����������ضϴ���\n" + ret;
			ret = ret.substr(0, 3000);
		}
		CQ_sendDiscussMsg(ac, fromDiscuss, ret.c_str());
	}
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=101 Ⱥ�¼�-����Ա�䶯
* subType �����ͣ�1/��ȡ������Ա 2/�����ù���Ա
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=102 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/ȺԱ�뿪 2/ȺԱ���� 3/�Լ�(����¼��)����
* fromQQ ������QQ(��subTypeΪ2��3ʱ����)
* beingOperateQQ ������QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=103 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/����Ա��ͬ�� 2/����Ա����
* fromQQ ������QQ(������ԱQQ)
* beingOperateQQ ������QQ(����Ⱥ��QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=201 �����¼�-���������
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=301 ����-�������
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=302 ����-Ⱥ���
* subType �����ͣ�1/����������Ⱥ 2/�Լ�(����¼��)������Ⱥ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

///*
//* �˵������� .json �ļ������ò˵���Ŀ��������
//* �����ʹ�ò˵������� .json ���˴�ɾ�����ò˵�
//*/
//CQEVENT(int32_t, __menuA, 0)() {
//	//MessageBoxA(NULL, "����menuA�����������봰�ڣ����߽�������������", "" ,0);
//	return 0;
//}
//
//CQEVENT(int32_t, __menuB, 0)() {
//	//MessageBoxA(NULL, "����menuB�����������봰�ڣ����߽�������������", "" ,0);
//	return 0;
//}
