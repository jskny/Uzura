/*
 *	Uzura
 *	カラを破ってもう一度
 *	2012 / 08 / 06
 *	jskny
*/


#include	"uzura.h"
#include	"actor.h"


using namespace std;


int WINDOW_W = 640;
int WINDOW_H = 480;


UzuraLogic* g_uzura = NULL;


// ハッシュ値を取得 ・・・ すべて小文字化する。
unsigned int GetHash(const char* const str)
{
	AEN_ASSERT_KILL(str, "null pointer.");


	unsigned int hash = aen::hash::AdlerStringLower(str);
	return (hash);
}


static int g_score = 0, g_scoreH;
// スコア
void SetScore(int s)
{
	if (s > g_scoreH) {
		g_scoreH = s;
	}


	g_score = s;
}


int GetScore(void)
{
	return (g_score);
}


int GetScoreHigh(void)
{
	return (g_scoreH);
}


float g_angle = 0;
float g_angle2 = 0;
int g_count = 0;
int g_countC = 0;
int g_prevT = 0;


// テンポによる拡大率を取得。
float GetScale(int BPM, float num)
{
const float seedTime = (60.0f / BPM) * 1000;
const float seed = ((GetNowCount() - g_prevT) / seedTime);
	return (seed * num);
}


// オーバーライド版。手抜き。
float GetScale(float num)
{
	return (GetScale(g_uzura->getBpm(), num));
}


void InitBackEn(void)
{
	g_angle = 0;
	g_angle2 = 0;
	g_count = 0;
	g_countC = 0;
	g_prevT = 0;


	if (DxLib_IsInit()) {
		g_prevT = GetNowCount();
	}


}


// BPM system tiem update
// 時刻更新。
void UpdateBpmSystem(const int BPM)
{
const float seedTime = (60.0f / BPM) * 1000;
	if ((GetNowCount() - g_prevT) > seedTime) {
		g_prevT = GetNowCount();
	}

	if ((GetNowCount() - g_prevT) > seedTime) {
		g_prevT = GetNowCount();
	}


}


void DrawBackEn(const int BPM, const int MAX, const int colorLineI, const int colorLineO)
{
using namespace aen::gl;


const float seedTime = (60.0f / BPM) * 1000;
static float s_addR = 0;
	if ((GetNowCount() - g_prevT) > seedTime) {
		s_addR = MAX;
		g_prevT = GetNowCount();
	}
	else {
		float seed = ((GetNowCount() - g_prevT) / seedTime);
		s_addR = MAX * seed;
// cout << "R:" << s_addR << endl;
	}


	int backColor = 0;
const	int backColorMax = 0x20;
	if ((GetNowCount() - g_prevT) > seedTime) {
		backColor = GetColor(backColorMax, backColorMax, backColorMax);
		g_prevT = GetNowCount();
	}
	else {
		backColor = (backColorMax) * ((GetNowCount() - g_prevT) / seedTime);
		backColor = 0xFF - backColor;
		backColor = GetColor(backColor, backColor, backColor);
// cout << "R:" << s_addR << endl;
	}


//	SetDrawScreen(g_screen);
//	DrawBox(0, 0, WINDOW_W, WINDOW_H, backColor, TRUE);


	// MEMO:
	// 2012 / 08 / 15
	// 時計みたいで面白いので残す。
	g_angle += GetRad(2 * GetScale(2));
	g_angle2 += GetRad(3 * GetScale(2));
	//
	if (g_count < 0) {
		g_count = 0;
	}

	// 円の大きさ = スケール
	if (g_countC < 0) {
		g_countC = 0;
	}


	int color = colorLineI;//GetColor(0xFF, 0x00, 0x00);
	int tmpCountC = g_countC;
	g_countC += GetScale(10);
	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 3, -g_angle, 20 + g_countC, color, 2, 4);
	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 10, g_angle, 50 + g_countC, color, 3, 4);
	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 8, -g_angle2 + GetRad(18), 100 + g_countC, color, 2, 3);
	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 7, g_angle2, 140 + g_countC, color, 2, 2);
	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 7, -g_angle, 180 + g_countC, color, 1, 1);
	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 14, g_angle, 220 + g_countC, color, 2, 1);

	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 9, -g_angle2, 180 + g_countC, color, 2, 2);


	// 円陣
static float s_angleEn = 0.0f;
static float s_angleEn2 = 0.0f;
static int s_baseEn = 300;
	s_angleEn += GetRad(5);
	s_angleEn2 += GetRad(10);
	if (s_angleEn > GetRad(360)) {
		s_angleEn = 0;
	}
	if (s_angleEn2 > GetRad(360)) {
		s_angleEn2 = 0;
	}


	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 32, s_angleEn, s_baseEn + g_countC + s_addR, colorLineO, 4, 1);
	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 32, -s_angleEn, s_baseEn - 40 + g_countC + s_addR, colorLineO, 2, 1);
	DrawPolygon2d(WINDOW_W / 2, WINDOW_H / 2, 64, -s_angleEn2, s_baseEn - 50 + g_countC + s_addR, colorLineO, 6, 1);
//	DrawPolygonN(WINDOW_W / 2, WINDOW_H / 2, 180, -s_angleEn, s_baseEn + g_countC, GetColor(0xFF, 0x00, 0x00), 85, 1);


	// 値を元に戻す、円陣を弄りたくはなので
	g_countC = tmpCountC;


/*
static bool s_flagInvert = false;
	if (IsNowHitKey(KEY_INPUT_A)) {
		s_flagInvert = !s_flagInvert;
	}


	if (s_flagInvert) {
		// 色調反転
		GraphFilter(g_screen, DX_GRAPH_FILTER_INVERT);
	}


	// 低解像度画面生成。
	GraphFilterBlt(g_screen, g_screenLow, DX_GRAPH_FILTER_DOWN_SCALE, 8);
	if (g_count != 0) {
		GraphFilter(g_screenLow, DX_GRAPH_FILTER_GAUSS, 16, g_count);
	}


	// 描画対象を戻す。
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0, 0, g_screen, FALSE);


	if (g_count != 0) {
SetDrawBlendMode(DX_BLENDMODE_ADD, 0xFF/2);
		// 描画モードをバイリニアフィルタリングにする( 拡大したときにドットがぼやけるようにする )
		SetDrawMode(DX_DRAWMODE_BILINEAR);
		DrawExtendGraph(0, 0, WINDOW_W, WINDOW_H, g_screenLow, FALSE);
		// 描画モードを二アレストに戻す
		SetDrawMode(DX_DRAWMODE_NEAREST);
SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0xFF);
	}
*/

	return;
}


// 後ろでピョイピョイやっている線分を出力
void DrawBackLine(void)
{
static int g_y = 0;
	const int size = 128;
	const float n = size-GetScale(size);


	g_y++;
	if (g_y > size) {
		g_y = 0;
	}
	for (int y = 0; y < WINDOW_H / size + 1; ++y) {
		const float sy = y * size + g_y;
		for (int x = 0; x < WINDOW_W / size + 1; ++x) {
			float sx = x * size;
			DrawLine(sx, sy, sx+n, sy, GetColor(0xFF - n, 0, 0));
		}
	}


	return;
}


// 後ろのバー
void DrawBackBox(int r, int g, int b)
{
using namespace aen::gl;
	const int size = GetScale(WINDOW_H);
	const float n = GetScale(1);
	const int c = GetColor(r*n, g*n, b*n);


	// original
Blend::push();
Blend::set(Blend::BT_ADD, 0xFF / 2);
	Blend::push();
	Blend::set(Blend::BT_ADD, 0xFF / 2);
		DrawBox(
			0, WINDOW_H - size,
			WINDOW_W, WINDOW_H,
			c,
			TRUE);
	Blend::pop();

		// / 2
	Blend::push();
	Blend::set(Blend::BT_ADD, 0xFF / (2*2));
		DrawBox(
			0, WINDOW_H - size / (2*1),
			WINDOW_W, WINDOW_H,
			c,
			TRUE);
	Blend::pop();

		// / 4
	Blend::push();
	Blend::set(Blend::BT_ADD, 0xFF / (2*4));
		DrawBox(
			0, WINDOW_H - size / (2*2),
			WINDOW_W, WINDOW_H,
			c,
			TRUE);
	Blend::pop();
Blend::pop();

}


// 読込中画面の出力
void DrawLoadingScene(void)
{
static	aen::gl::Picture back;
static	aen::gl::Picture m;
static	int count = 0;
static	bool flagC = false;
static	float angle = 0;


	if (!back.isLoad()) {
		back.load("./data/graphics/nowloading/back.png");
	}


	if (!m.isLoad()) {
		m.load("./data/graphics/nowloading/m.png");
	}


	angle += GetRad(3);
	if (angle > GetRad(360)) {
		angle = 0;
	}


	if (count > 80) {
		flagC = true;
		count = 80;
	}
	if (count < -80) {
		flagC = false;
		count = -80;
	}


	// ふわふわ～
	count += (flagC ? -3 : 2);


using namespace aen::gl;
	back.draw(0, 0, 0);
	if (abs(count) < 20) {
		DrawString(WINDOW_W - 200, WINDOW_H - 150 - count - 16, "ピヨー", GetColor(0xFF, 0x00, 0x00));
	}


Blend::push();
Blend::set(Blend::BT_ADD, 127);
DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0, 0, 50 + GetRand(30)), TRUE);
Blend::pop();
	m.draw(WINDOW_W - 100, WINDOW_H - 150 - count, 0, angle, 1);
}


// バック
//グリッドを引きます XY面用
void DrawGridXY(VECTOR offset, int segment, float interval, float z, int color)
{
	offset.x -= interval*segment/2;
	offset.y -= interval*segment/2;
	for(int i=0;i<segment;i++){
		for(int j=0;j<segment;j++){
			DrawLine3D(VAdd(VGet(i*interval, j*interval, z), offset),
					   VAdd(VGet((i+1)*interval, j*interval, z), offset), color);
			DrawLine3D(VAdd(VGet(i*interval, j*interval, z), offset),
					   VAdd(VGet(i*interval, (j+1)*interval, z), offset), color);
		}
	}
}


//グリッドを引きます XZ面用
void DrawGridXZ(VECTOR offset, int segment, float interval, float y, int color)
{
	offset.x -= interval*segment/2;
	offset.z -= interval*segment/2;
	for(int i=0;i<segment;i++){
		for(int j=0;j<segment;j++){
			DrawLine3D(VAdd(VGet(i*interval, y, j*interval), offset),
					   VAdd(VGet((i+1)*interval, y, j*interval), offset), color);
			DrawLine3D(VAdd(VGet(i*interval, y, j*interval), offset),
					   VAdd(VGet(i*interval, y, (j+1)*interval), offset), color);
		}
	}
}


//3D空間中にグリッドを引きます
void DrawGrid(VECTOR offset, int segment, float interval, int color, int brightness)
{
	SetDrawBlendMode(DX_BLENDMODE_ADD, brightness);
	for(int i=0;i<segment;i++){
		DrawGridXY(offset, segment, interval, interval*i-(interval*segment/2), color);
		DrawGridXZ(offset, segment, interval, interval*i-(interval*segment/2), color);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


//---------------------------------------------------------
// UzuraLogic
//---------------------------------------------------------


UzuraLogic::UzuraLogic() :
	m_bpm(0), m_flagShowProfile(false), m_flagDebug(false)
{
	this->getManagerEvent()->clear();
	this->getManagerProfile()->clear();


	this->m_handleSubsystem = -1;


	// クラッシュイベントの受信。
	auto listener = boost::shared_ptr<aen::system::event::Listener>(new EventListenerSystem());
	this->getManagerEvent()->addListener(listener, aen::hash::AdlerStringLower("crash"));
	this->getManagerEvent()->addListener(listener, aen::hash::AdlerStringLower("PLAYER-DAMAGE"));
}


UzuraLogic::~UzuraLogic()
{
	this->getManagerEvent()->clear();
	this->getManagerProfile()->clear();
}


// EvenShooter システムイベントリスナー
bool EventListenerSystem::handleEvent(aen::system::event::Event* event)
{
cout << "EventListenerSystem::handleEvent -> OutputEvent listener : ";
cout << event->getType() << "," << event->getValue() << "," << event->getStrValue() << endl;


	// イベントの中身、
	// type		種類
	// value	数値データ ( 座標など )
	// strValue	文字列データ ( マップ名とか、 )
const	int	eventClash = aen::hash::AdlerStringLower("crash");
const	int	eventPlayerDamage = aen::hash::AdlerStringLower("PLAYER-DAMAGE");
	if (event->getType() == eventClash) {
		// Again 強制終了イベント
		g_uzura->crash(event->getStrValue().c_str());
		return (true);
	}
	else if (event->getType() == eventPlayerDamage) {
cout << "Event listener PLAYER Damage" << endl;
		return (true);
	}


	return (true);
}


bool UzuraLogic::initialize(void)
{
	// 処理
	this->m_flagShowProfile = false;
	this->m_flagDebug = false;


	// 基底クラス呼び出し
aen::Logic::initialize();
	return (true);
}


bool UzuraLogic::finalize(void)
{
	// 処理。
	CloseNetWork(g_uzura->getHandleSubsystem());
	g_uzura->setHandleSubsystem(-1);


	// 基底クラス呼び出し
aen::Logic::finalize();
	return (true);
}


// システムループ呼び出し
bool UzuraLogic::systemLoop(void)
{
	if (!g_uzura->getWindow()->windowLoopNonClearNonFlip()) {
		return (false);
	}


	// シーンを使う。
	if (!g_uzura->getWindow()->windowLoop()) {
		return (false);
	}


	// 回る～回るよ～
	g_uzura->getManagerEvent()->tick(1000);
	g_uzura->getManagerProfile()->restart();
	return (true);
}


// プロファイル結果の出力
void UzuraLogic::drawProfile(int fontSize/* = 16*/)
{
	// プロファイル結果の出力。
	auto vec = g_uzura->getManagerProfile()->getProfiles();
/*	for (auto itr = vec.begin(); itr != vec.end(); ++itr) {
		for (int i = 0; i < itr->getStep()+1; ++i) {
			if (i != 0) {
				cout << "-";
			}
			else {
				cout << ":";
			}
		}


		cout << itr->getName() << " [" << itr->getTime() << "]" << endl;
	}*/


	int y = 1;
	int num = 0;
	for (auto itr = vec.begin(); itr != vec.end(); ++itr) {
		if (vec.size() > 35) {
			// プロファイルの数が 35 個より多くなったなら、
			// レベルの低いプロファイル情報は出力しない。
			if (itr->getLevel() < 0) {
				itr++;
				continue;
			}
		}


		for (int i = 0; i < itr->getStep()+1; i++) {
			if (i != 0) {
				DrawString(i*8, y*fontSize, "-", GetColor(0x50, 0xD0, 0xFF));
			}
			else {
				DrawString(i*8, y*fontSize, ":", GetColor(0x50, 0xD0, 0xFF));
			}
		}


		DrawFormatString(
			(itr->getStep()+1) * 8, y * fontSize,
			itr->getColor(),
			"%s : [%ld]", itr->getName().c_str(), itr->getTime());


		num += itr->getTime();
		y++;
	}


	DrawFormatString(0, 0, GetColor(0xFF, 0x00, 0x00), "----- Profile ( %d sec ) --------------------", num);
	DrawFormatString(WINDOW_W - 80, WINDOW_H - fontSize, GetColor(0xFF, 0x00, 0x00), "FPS %2.2f", this->getWindow()->getFps());
}


// カメラ行列適応。
void UzuraLogic::updateCameraMatrix(const VECTOR v/* = VGet(0, 30, -100) */)
{
	// 気分でカメラ遅延。
const int MAX = 50;
static MATRIX g_cameraLog[MAX] = { MGetIdent() };
static int g_now = -1;


	if (g_mapActors[HASH_KEY_PLAYER].get() == 0) {
		g_mapActors[HASH_KEY_PLAYER] = boost::shared_ptr<ActorParam>(new ActorParam());
	}


	MATRIX org = g_mapActors[HASH_KEY_PLAYER]->getMatrix();


	if (g_now == -1) {
		for (int i = 0; i < MAX; ++i) {
			g_cameraLog[i] = MGetIdent();
		}


		g_now = 0;
	}


	int index = g_now % MAX;

	// 記録。
	MATRIX test = MGetIdent();
	g_cameraLog[index] = org;


	test = g_cameraLog[(g_now - 7) % MAX];
	g_now++;


	if (g_mapActors[HASH_KEY_PLAYER].get() == 0) {
		g_mapActors[HASH_KEY_PLAYER] = boost::shared_ptr<ActorParam>(new ActorParam());
	}


	test.m[3][0] = org.m[3][0];
	test.m[3][1] = org.m[3][1];
	test.m[3][2] = org.m[3][2];
//	test = MInverse(test);
	MATRIX m = test;
	MATRIX tmp = m;

	// 平行移動要素をキャンセルして姿勢行列を取り出す
	tmp.m[3][0] = tmp.m[3][1] = tmp.m[3][2] = 0.0f;
	// V(0,30,-100)を姿勢行列で変形させて
	tmp = MMult(MGetTranslate(v), tmp);
	// 平行移動要素を書き戻し
	m = MMult(tmp, MGetTranslate(VGet(m.m[3][0], m.m[3][1], m.m[3][2])));


	// 以下の中身でセット SetCameraViewMatrix(MInverse(g_uzura->getCameraMatrix()));
	this->setCameraMatrix(m);
}


// 基準線
void DrawStandardLine3d(const VECTOR& c)
{
	// X
	for (int z = -15; z < 15; z++) {
		DrawLine3D(
			VAdd(c, VGet(-750, 0, z*50)),
			VAdd(c, VGet(0, 0, z*50)),
			GetColor(0xFF, 0x50, 0x50));

		DrawLine3D(
			VAdd(c, VGet(0, 0, z*50)),
			VAdd(c, VGet(750, 0, z*50)),
			GetColor(0x50, 0x50, 0xFF));
	}
	DrawLine3D(VAdd(c, VGet(-750, 0, 15*50)), VAdd(c, VGet(0, 0, 15*50)), GetColor(0xFF, 0x50, 0x50));
	DrawLine3D(VAdd(c, VGet(0, 0, 15*50)), VAdd(c, VGet(750, 0, 15*50)), GetColor(0x50, 0x50, 0xFF));

	// Y
/*	for (int y = -16; y < 16; y++) {
		DrawLine3D(VGet(-50*16, y*50, 0), VGet(50*16, y*50, 0), GetColor(0x80, 0xAA, 0x00));
		for (int x = -16; x < 16; x++) {
			DrawLine3D(VGet(x*50, -50*16, 0), VGet(x*50, 50*16, 0), GetColor(0xAA, 0x80, 0x00));
		}
	}*/

	DrawLine3D(VAdd(c, VGet(0, -750, 0)), VAdd(c, VGet(0, 0, 0)), GetColor(0xFF, 0xFF, 0x00));
	DrawLine3D(VAdd(c, VGet(0, 750, 0)), VAdd(c, VGet(0, 0, 0)), GetColor(0x00, 0xFF, 0xFF));
	// Z
	for (int x = -15; x < 15; x++) {
		DrawLine3D(VAdd(c, VGet(x*50, 0, -750)), VAdd(c, VGet(x*50, 0, 0)), GetColor(0xFF, 0xA0, 0x00));
		DrawLine3D(VAdd(c, VGet(x*50, 0, 0)), VAdd(c, VGet(x*50, 0, 750)), GetColor(0x00, 0xAF, 0xFF));
	}
	DrawLine3D(VAdd(c, VGet(15*50, 0, -750)), VAdd(c, VGet(15*50, 0, 0)), GetColor(0xFF, 0xA0, 0x00));
	DrawLine3D(VAdd(c, VGet(15*50, 0, 0)), VAdd(c, VGet(15*50, 0, 750)), GetColor(0x00, 0xAF, 0xFF));
}


// モデルを読み込む。
aen::gl::Model GetModel(const std::string& urlFile)
{
static	std::hash_map<std::string, aen::gl::Model>	s_handles;
	aen::gl::Model a;


	auto itr = s_handles.find(urlFile);
	if (itr != s_handles.end()) {
		// すでに読み込まれていれば、それを返す。
		a = itr->second;
		if (a.isLoad()) {
			return (a);
		}
	}


	if (!a.load(urlFile.c_str())) {
AEN_STREAM_LOG_ERROR() << "Can not found to '" << urlFile << "'." << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Can not found to '" << urlFile << "'." << endl;
		cerr << "Can not found to '" << urlFile << "'." << endl;
		return (a);
	}


	if (!a.isLoad()) {
AEN_STREAM_LOG_ERROR() << "Can not load to '" << urlFile << "'." << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Can not load to '" << urlFile << "'." << endl;
		cerr << "Can not load to '" << urlFile << "'." << endl;
		return (a);
	}


	s_handles[urlFile] = a;
	return (a);
}


// テクスチャの読み込み
aen::gl::Picture GetTexture(const std::string& urlFile)
{
static	std::hash_map<std::string, aen::gl::Picture>	s_handles;
	aen::gl::Picture a;


	auto itr = s_handles.find(urlFile);
	if (itr != s_handles.end()) {
		// すでに読み込まれていれば、それを返す。
		a = itr->second;
		if (a.isLoad()) {
			return (a);
		}
	}


	if (!a.load(urlFile.c_str())) {
AEN_STREAM_LOG_ERROR() << "Can not found to '" << urlFile << "'." << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Can not found to '" << urlFile << "'." << endl;
		cerr << "Can not found to '" << urlFile << "'." << endl;
		return (a);
	}


	if (!a.isLoad()) {
AEN_STREAM_LOG_ERROR() << "Can not load to '" << urlFile << "'." << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Can not load to '" << urlFile << "'." << endl;
		cerr << "Can not load to '" << urlFile << "'." << endl;
		return (a);
	}


	s_handles[urlFile] = a;
	return (a);
}


// テクスチャの読み込み(設定ファイルの読み込みあり。)
aen::gl::Picture GetTexture(const std::string& urlFile, const std::string& urlConf)
{
static	std::hash_map<std::string, aen::gl::Picture>	s_handles;
	aen::gl::Picture a;


	auto itr = s_handles.find(urlFile);
	if (itr != s_handles.end()) {
		// すでに読み込まれていれば、それを返す。
		a = itr->second;
		if (a.isLoad()) {
			return (a);
		}
	}


	gscripter::Scripter scripter;
	// スクリプト読み込み、実行。
	if (!scripter.load(urlConf)) {
AEN_STREAM_LOG_ERROR() << "GScripter Script load error : " << urlConf << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script load error : " << urlConf << endl;
		cerr << "GScripter Script load error : " << urlConf << endl;
		return (a);
	}
	else if (!scripter.lex()) {
AEN_STREAM_LOG_ERROR() << "GScripter Script compile error : " << urlConf << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script compile error : " << urlConf << endl;
		cerr << "GScripter Script compile error : " << urlConf << endl;
		return (a);
	}
	else if (!scripter.compile()) {
AEN_STREAM_LOG_ERROR() << "GScripter Script compile error : " << urlConf << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script compile error : " << urlConf << endl;
		cerr << "GScripter Script compile error : " << urlConf << endl;
		return (a);
	}


	while (!scripter.isScriptEnd()) {
		if (!scripter.run()) {
AEN_STREAM_LOG_ERROR() << "GScripter Script running error : " << urlConf << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script running error : " << urlConf << endl;
cerr << "GScripter Script running error : " << urlConf << endl;
			break;
		}
	}


	int nG = scripter.getValue("numG");
	int nGx = scripter.getValue("numGx");
	int nGy = scripter.getValue("numGy");
	int imageOneW = scripter.getValue("imageOneW");
	int imageOneH = scripter.getValue("imageOneH");


	if (!a.load(urlFile.c_str(), nG, nGx, nGy, imageOneW, imageOneH)) {
AEN_STREAM_LOG_ERROR() << "Can not found to '" << urlFile << "'." << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Can not found to '" << urlFile << "'." << endl;
		cerr << "Can not found to '" << urlFile << "'." << endl;
		return (a);
	}


	if (!a.isLoad()) {
AEN_STREAM_LOG_ERROR() << "Can not load to '" << urlFile << "'." << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Can not load to '" << urlFile << "'." << endl;
		cerr << "Can not load to '" << urlFile << "'." << endl;
		return (a);
	}


	s_handles[urlFile] = a;
	return (a);
}


// 音声ファイルの読み込み。
aen::gl::sound::Player GetSound(const std::string& urlFile, const std::string& urlConf)
{
static	std::hash_map<std::string, aen::gl::sound::Player>	s_handles;
	aen::gl::sound::Player a;


	auto itr = s_handles.find(urlFile);
	if (itr != s_handles.end()) {
		// すでに読み込まれていれば、それを返す。
		a = itr->second;
		if (a.isLoad()) {
			return (a);
		}
	}


	string extendP;
if (!urlConf.empty()) {
	gscripter::Scripter scripter;
	// スクリプト読み込み、実行。
	if (!scripter.load(urlConf)) {
AEN_STREAM_LOG_ERROR() << "GScripter Script load error : " << urlConf << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script load error : " << urlConf << endl;
		cerr << "GScripter Script load error : " << urlConf << endl;
		return (a);
	}
	else if (!scripter.lex()) {
AEN_STREAM_LOG_ERROR() << "GScripter Script compile error : " << urlConf << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script compile error : " << urlConf << endl;
		cerr << "GScripter Script compile error : " << urlConf << endl;
		return (a);
	}
	else if (!scripter.compile()) {
AEN_STREAM_LOG_ERROR() << "GScripter Script compile error : " << urlConf << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script compile error : " << urlConf << endl;
		cerr << "GScripter Script compile error : " << urlConf << endl;
		return (a);
	}


	while (!scripter.isScriptEnd()) {
		if (!scripter.run()) {
AEN_STREAM_LOG_ERROR() << "GScripter Script running error : " << urlConf << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "GScripter Script running error : " << urlConf << endl;
cerr << "GScripter Script running error : " << urlConf << endl;
			break;
		}
	}


	extendP = scripter.getString("ftype");
}


	std::ostringstream oss;
	oss << urlFile << "." << extendP;


	// 音源情報
	aen::gl::sound::Player::ModeSound lt = a.MS_WAVE;
	if (extendP == "ogg" || extendP == "mp3") {
		lt = a.MS_ARCHIVE;
	}


	if (!a.load(oss.str().c_str(), lt)) {
AEN_STREAM_LOG_ERROR() << "Can not found to '" << urlFile << "'." << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Can not found to '" << urlFile << "'." << endl;
		cerr << "Can not found to '" << urlFile << "'." << endl;
		return (a);
	}


	if (!a.isLoad()) {
AEN_STREAM_LOG_ERROR() << "Can not load to '" << urlFile << "'." << endl;
AEN_STREAM_LOG_DEBUG(aen::DEBUG_LOG_LEVEL_GAMESYSTEM) << "Can not load to '" << urlFile << "'." << endl;
		cerr << "Can not load to '" << urlFile << "'." << endl;
		return (a);
	}


	s_handles[urlFile] = a;
	return (a);
}

