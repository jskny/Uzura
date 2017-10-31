/*
 *	Uzura
 *	カラを破ってもう一度
 *	2012 / 11 / 13
 *	jskny
*/


#include	<iostream>
#include	<sstream>
#include	<string>
#include	<vector>
#include	<map>


#include	<DxLib.h>


#include	"shell.h"


using namespace std;


static bool g_flagRunningShell = false;

static const int MAX = 30;
static int g_index = 0;
static char g_log[MAX][255];
static char g_buf[255];


// uzura.h は読み込みたくないので、
static const int WINDOW_W = 800, WINDOW_H = 600;


class pFuncBinder
{
public:
	int (*pFunc) (int n, const std::vector<std::string>& args);


};


map<string, pFuncBinder> g_mapShellFunc;


// 初期化。
void ShellReset(void)
{
	g_mapShellFunc.clear();


	g_index = 0;
	strcpy(g_buf, "");
	for (int i = 0; i < MAX; ++i) {
		strcpy(g_log[i], "");
	}
}


// 関数の登録。
void ShellAttachFunction(std::string name, int (*pFunc) (int n, const std::vector<std::string>& args))
{
	pFuncBinder t;
	t.pFunc = pFunc;
	g_mapShellFunc[name] = t;
}


// aen_util.cpp より
/*! 文字列の中にその文字があるか検索。あれば存在するバイト位置、なければ -1
 * @brief		文字列中の文字を検索、あれば発見バイト位置、なければ -1
 * @param source	文字列
 * @param k		検索する文字
 * @param n		発見をスキップする。
 * @return		発見 バイト位置、存在しない -1
 */
int GetFoundStr(const char* const s, char k, int n)
{
	int count = 0;


	if (!s) {
		return (-1);
	}


	int l = strlen(s);
	for (int i = 0; i < l; ++i) {
		if (s[i] == k) {
			// 発見したらば、その場所を返す。
			if (count == n) {
				return (i);
			}
			else {
				count++;
			}
		}
	}


	return (-1);
}


// シェルが動いていれば true を返す。というか、exit 入力時に false になるだけ。
bool IsRunningShell(void)
{
	return (g_flagRunningShell);
}


// シェル稼働フラグ・・・
void SetFlagRunningShell(bool flag)
{
	g_flagRunningShell = flag;
}


// 文字列を追加。
void ShellAddLine(const char* const line)
{
	if (!line) {
cout << "ShellAddLine >> null pointer." << endl;
		return;
	}


	// 更新処理。
	if (g_index < MAX) {
		strcpy(g_log[g_index], line);
		g_index++;
	}
	else {
		// すでにいっぱいだった。
		for (int i = 1; i < MAX; ++i) {
			strcpy(g_log[i - 1], g_log[i]);
		}

		strcpy(g_log[MAX - 1], line);
	}


	return;
}


// 一つ一つのコマンド
void ShellDoCommand(const std::vector<string>& vec)
{
	for (int i = 0; i < vec.size(); ++i) {
		string k = vec.at(i);

		if (k == "exit") {
			SetFlagRunningShell(false);
			return;
		}
		else if (k == "echo") {
			if (i+1 < vec.size()) {
				ShellAddLine(vec.at(i+1).c_str());
				++i;
			}
			else {
				ShellAddLine("Command echo parameter alignment is unjust.");
			}

		}
		else if (k == "cls") {
			g_index = 0;
			for (int i = 0; i < MAX; ++i) {
				strcpy(g_log[i], "");
			}
		}
		else {
			auto itr = g_mapShellFunc.find(vec.at(i));
			if (itr != g_mapShellFunc.end()) {
				// 登録されている関数の呼び出し。
				int ret = (*itr->second.pFunc)(i, vec);
				if (ret < 0) {
					std::ostringstream oss;
					oss << "Command '" << vec.at(i) << "' is fail maybe.";
					ShellAddLine(oss.str().c_str());
					continue;
				}
				else {
					i += ret;
					continue;
				}
			}


			std::ostringstream oss;
			oss << "Command '" << vec.at(i) << "' is not found.";
			ShellAddLine(oss.str().c_str());
		}

	}
}


// コマンドを処理する。
void ShellDoLine(const char* const line)
{
	// 文字列切り分け。
	char parse[] = " ";
	char buf[255];
	strcpy(buf, line);


	int ft = GetFoundStr(buf, '"', 0);
	vector<string> args;
if (ft == -1) {
	// ダブルクォーテーション " が無いバージョン
	char* token = strtok(buf, parse);
	while (token != NULL ) {
		args.push_back(token);
		token = strtok(NULL, parse);
	}
}
else {
	// ダブルクォーテーション、一つしか対応しないからな・・・
	int et = GetFoundStr(buf, '"', 1);
	if (et == -1) {
		ShellAddLine("Can't found end double-quotation.");
		return;
	}


static	char buf2[255];
	int count = 0;
	// 最初と最後のクォーテーションを取るため。
	// et - 1 としていないのは、 式が <= ではなく < だから。
	for (int i = ft+1; i < et; ++i) {
		buf2[count] = buf[i];
		++count;
	}
	buf2[count] = '\0';


	// さぁ引数切り分けだ。
	int size = strlen(buf);
static	char buf3[255];
	// space の処理用。 以前スペースが有った位置を記録。
	int sf = 0;
	bool flagFastTakenPush = false;
	count = 0;
	while (count < size) {
		if (count == ft) {
			// " の先頭に来たら最後まで飛ばす。
			count = et;
			args.push_back(buf2);
			continue;
		}


		if (buf[count] == ' ') {
			if (!flagFastTakenPush) {
				// 最初の切り離し。
				int tmp = 0;
				for (int i = 0; i < count; ++i) {
					buf3[tmp] = buf[i];
					++tmp;
				}
				buf3[tmp] = '\0';
cout << "PUSH : " << buf3 << endl;
				args.push_back(buf3);


				++count;
				flagFastTakenPush = true;
				continue;
			}


			if (sf != 0) {
				// スペースの開始から終わりまでをコピー
				int tmp = 0;
				for (int i = sf; i < count; ++i) {
					buf3[tmp] = buf[i];
					++tmp;
				}
				buf3[tmp] = '\0';
cout << "PUSH : " << buf3 << endl;
				args.push_back(buf3);


				sf = 0;
			}
			else {
				sf = count;
			}
		}


		++count;
	}


	// 残りを詰める。
	if (sf != 0) {
		int tmp = 0;
		for (int i = sf; i < count; ++i) {
			buf3[tmp] = buf[i];
			++tmp;
		}
		buf3[tmp] = '\0';
cout << "PUSH : " << buf3 << endl;
		args.push_back(buf3);
	}
}


	ShellDoCommand(args);
	return;
}


// 出力
void ShellDraw(void)
{
	DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0x00, 0x00, 0x20),TRUE);


	DrawString(0, 0, "* Uzura Engine Shell", GetColor(0x00, 0xFF, 0x00));
	DrawLine(0, 20, WINDOW_W, 20, GetColor(0xFF, 0xFF, 0x00));
	DrawLine(5, 20, 5, WINDOW_H - 16, GetColor(0xFF, 0xA0, 0xA0));

	DrawString(0, WINDOW_H - 16, "shell#", GetColor(0, 0xFF, 0));


	// 履歴の出力
	if (g_index < MAX) {
		for (int i = 0; i < g_index; ++i) {
			DrawString(10, 25 + i*16, g_log[i], GetColor(0x50, 0xFF, 0x50));
		}
		DrawLine(10, 25+g_index*16, WINDOW_W, 25+g_index*16, GetColor(0xFF, 0xFF, 0xFF));
	}
	else {
		for (int i = 0; i < MAX; ++i) {
			DrawString(10, 25 + i*16, g_log[i], GetColor(0x50, 0xFF, 0x50));
		}
		DrawLine(10, 25+MAX*16, WINDOW_W, 25+MAX*16, GetColor(0xFF, 0xFF, 0xFF));
	}


}


// 入力
void ShellInput(void)
{
	DrawString(0, WINDOW_H - 16, "shell#", GetColor(0, 0xFF, 0));
	int rc = KeyInputString(60, WINDOW_H - 16, 200, g_buf, TRUE);
	if (rc != 1 && rc != 2) {
		// まだ、入力の途中。
		return;
	}
	else {
		// 入力完了！
		if (strcmp(g_buf, "") == 0) {
			// 空行
			ShellAddLine(g_buf);
		}


		// コマンド処理。
		ShellDoLine(g_buf);
	}

}

