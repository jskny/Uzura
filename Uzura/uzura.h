/*
 *	Uzura
 *	カラを破ってもう一度
 *	2012 / 08 / 06
 *	jskny
*/


#pragma once

#ifndef __UZURA_UZURA_INCLUDE_GUARD_8739976_43_229091_21__
#define __UZURA_UZURA_INCLUDE_GUARD_8739976_43_229091_21__


#include	<aen.h>


// ホントは良くないのだが、
#define		GetRad(val)		aen::util::GetRad(val)


// ゲームロジック本体
class UzuraLogic;
extern	UzuraLogic*		g_uzura;


extern	int		WINDOW_W;
extern	int		WINDOW_H;


// ハッシュ値を取得 ・・・ すべて小文字化する。
extern	unsigned int	GetHash(const char* const str);

// モデルを読み込む。
extern	aen::gl::Model	GetModel(const std::string& urlFile);
extern	aen::gl::Picture	GetTexture(const std::string& urlFile);
// テクスチャの読み込み(設定ファイルの読み込みあり。)
extern	aen::gl::Picture	GetTexture(const std::string& urlFile, const std::string& urlConf);
// 音声ファイルの読み込み。
extern	aen::gl::sound::Player	GetSound(const std::string& urlFile, const std::string& urlConf);


extern	void		InitBackEn(void);
extern	void		DrawBackEn(const int BPM, const int MAX, const int colorLineI, const int colorLineO);
// テンポによる拡大率を取得。
extern	float		GetScale(int BPM, float num);
extern	float		GetScale(float num);
// 時刻更新。
extern	void		UpdateBpmSystem(const int BPM);
// 後ろでピョイピョイやっている線分を出力
extern	void		DrawBackLine(void);
// 後ろのバー
extern	void		DrawBackBox(int r, int g, int b);
// 読込中画面の出力
extern	void		DrawLoadingScene(void);
// 基準線
extern	void		DrawStandardLine3d(const VECTOR& c);


// Powered by Hoshino teacher
// 3D空間中にグリッドを引きます
extern	void		DrawGrid(VECTOR offset, int segment, float interval, int color, int brightness);


// スコア
extern	void		SetScore(int s);
extern	int		GetScore(void);
extern	int		GetScoreHigh(void);


class UzuraLogic :public aen::Logic
{
public:
	// プロファイルマネージャーを取得。
aen::profiler::Manager*	getManagerProfile(void) { return (&this->m_managerProfile); };
	// イベントマネージャー取得
aen::system::event::Manager*	getManagerEvent(void) { return (&this->m_managerEvent); };


	bool		initialize(void);
	bool		finalize(void);


	// システムループ呼び出し
	bool		systemLoop(void);
	// プロファイル結果の出力
	void		drawProfile(int fontSize = 16);
	// プロファイルを出力するか否か、 true 出力する。
	bool		isShowProfile(void) const { return (this->m_flagShowProfile); };
	void		setFlagShowProfile(bool flag) { this->m_flagShowProfile = flag; };


	int		getBpm(void) const { return (this->m_bpm); };
	// 人間というか、こちらの再生エンジンなのかは分からないが、120BPM の曲に 120 だと遅れが発生していた。
	// + 5 したらシンクロし続けたので、+5
	// これに深い意味はございません。
	// 2012 / 09 / 03
	// jskny
	void		setBpm(int bpm) { this->m_bpm = bpm + 5; };


	// カメラ行列を取得
	MATRIX		getCameraMatrix(void) const { return (this->m_matrixCamera); };
	void		setCameraMatrix(const MATRIX& a)
	{
		this->m_matrixCamera = a;
		SetCameraViewMatrix(MInverse(this->getCameraMatrix()));
	}
	// カメラ行列適応。
	void		updateCameraMatrix(const VECTOR v = VGet(0, 30, -100));


	UzuraLogic();
	~UzuraLogic();


	// デバッグモード？
	bool		isDebug(void) const { return (this->m_flagDebug); };
	void		setFlagDebug(bool flag) { this->m_flagDebug = flag; };


	// ネットワークハンドル。
	void		setHandleSubsystem(int h) { this->m_handleSubsystem = h; };
	int		getHandleSubsystem(void) const { return (this->m_handleSubsystem); };


private:
	aen::profiler::Manager	m_managerProfile;
	// イベントマネージャー
	aen::system::event::Manager	m_managerEvent;
	// 重要なパラメーターであります！
	int		m_bpm;
	// プロファイルを出力するか、
	bool		m_flagShowProfile;
	bool		m_flagDebug;


	// カメラ行列
	MATRIX		m_matrixCamera;
	// サブシステムとの通信用ソケット
	int		m_handleSubsystem;


};


// EvenShooter システムイベントリスナー
class EventListenerSystem :public aen::system::event::Listener
{
public:
	bool		handleEvent(aen::system::event::Event* event);


};


#endif // __UZURA_UZURA_INCLUDE_GUARD_8739976_43_229091_21__

