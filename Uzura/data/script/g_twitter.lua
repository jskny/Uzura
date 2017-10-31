-- -------------------------------------
-- Uzura
-- ゲームメイン画面。
-- 注意
-- LuaPlus はデフォルト引数を処理してはくれないので、
-- Lua から DxLib を呼ぶときは隠しパラメーターも打つ必要がある。
-- 2012 / 09 / 03
-- jskny
-- -------------------------------------


local g_handleScreen = 0
local g_angleMouse = 0
local g_handleSoundBgm = 0
local g_handleBackplate = 0
local g_timer = 0
local g_waitTime = 1000*10000


-- シーン開始時に呼ばれる。
function ListenerInit()
	g_handleScreen = MakeScreen(WINDOW_W, WINDOW_H, FALSE)
	InitBackEn()

	-- アクター初期化。
	InitActors()
	InitEnemys()
	InitEffect()


	g_handleSoundBgm = Sound.load("test0199.ogg")
	g_handleBackplate = Model.load("plates/test/data.mqo")
	Config("bpm", 140)
	Config("fps", 60)


	InitCamera()
	InitCameraGame()
	InitPlayer()


	SetPlayerHp(1);
	SetPlayerPos(0, 0, 0);
	SetScore(0)


	g_timer = GetNowCount()
	return true
end


-- シーン終了時に呼ばれる。
function ListenerFinish()
	DeleteGraph(g_handleScreen)

	Sound.stop(g_handleSoundBgm)
--	Sound.delete(g_handleSoundBgm)
	return true
end


function GetTimer()
	return (math.floor((g_timer-(GetNowCount()-(g_waitTime)))/1000))
end


-- ツイート作成。
function CreateTweet(id)
	local tmp = GetVectorPlayer()
	local ray = GetPlayerRay();
	local s = 10000
	ray.x = ray.x * s
	ray.y = ray.y * s
	ray.z = ray.z * s


	-- それに自キャラの座標を加えて出来上がり。
	ray.x = ray.x + tmp.x + GetRand(5) * GetRand(500)
	ray.y = ray.y + tmp.y + GetRand(5) * GetRand(500)
	ray.z = ray.z + tmp.z + GetRand(5) *  GetRand(500)


	AddTweet(id, ray.x, ray.y, ray.z)
end


-- イベント処理
function ListenerChangeBpm(dataBag)
	Config("bpm", dataBag)
	print("Change BPM has succeed.")
	return true
end


-- アップデートイベントで呼ばれる。
local g_flagPlaySound = 0
function Update()
	-- Bpm system update.
	UpdateBpmSystem()


	-- 音声再生開始
	if g_flagPlaySound == 0 then
		Sound.play(g_handleSoundBgm, Sound.BGM)
		g_flagPlaySound = 1
	end


	if Keybord.isOn(Keybord.Q) == true then
		-- title へ戻る。
		SendEvent("change-scene", "g_title")
	end


	-- SetDrawScreen でリセットされたカメラ設定を復元する。
	-- カメラ 制御
	if Keybord.isOn(Keybord.LCONTROL) then
		local tmp = GetVectorPlayer()
		UpdateCamera(tmp.x, tmp.y, tmp.z)
	else
		UpdateCameraGame(0, 100, -200)
	end


	TaskUpdate()


	local tmp = GetVectorPlayer()


	-- 範囲オーバー処理
	-- ループさせずに、動けなくする。
	local r = 6000 * 10
	local r2 = 6000 * 7
	local ry = 4000 * 10
	if (tmp.x < -r) then
		tmp.x = tmp.x + r2
	end
	if (tmp.x > r) then
		tmp.x = tmp.x - r2
	end
	if (tmp.y < -ry) then
		tmp.y = tmp.y + ry
	end
	if (tmp.y > ry) then
		tmp.y = tmp.y - ry
	end
	if (tmp.z < -r) then
		tmp.z = tmp.z + r2
	end
	if (tmp.z > r) then
		tmp.z = tmp.z - r2
	end


	-- プレイヤー座標の更新。
	SetPlayerPos(tmp.x, tmp.y, tmp.z);


	-- 自キャラのライフが 0 なら死ぬ。
	if (GetPlayerHp() <= 0) then
		-- 死ね。
		SendEvent("change-scene", "g_over")
	end


	-- 時間切れェェェェェ！！！
	local itimer = math.floor((g_timer-(GetNowCount()-(g_waitTime)))/1000)
	if (itimer <= 0) then
		SendEvent("change-scene", "g_clear")
	end


	if (GetSubsystemDataLength() ~= 0) then
		-- 新しいツイート！！！
		local id = GetSubsystemData()
print("Got a new tweet : " .. id)
		CreateTweet(id)
	end


	return true
end


-- アップデートイベントで呼ばれる。
function Draw()
	for z = 1, 10 do
		for x = 1, 10 do
			Model.setPos(g_handleBackplate, -6000 * 5 + 6000*x, -100, -6000 * 5 + 6000*z)
			Model.draw(g_handleBackplate)
		end
	end


	-- デバッグよう基準線。
	DrawStandardLine3d(0, 0, 0)
	local tmp = GetVectorPlayer()
--	DrawStandardLine3d(tmp.x, tmp.y, tmp.z)


	local max = 10
	local interval = 500
Blend.push()
Blend.set(Blend.ADD, 100)
	DrawGridCamera(max, interval, GetColor(160, 160, 120 + GetScale(120)), 96)
Blend.pop()


	--タスクシステムファイヤー
	TaskDraw()


	-- スコア出力
	DrawScore(WINDOW_W - 200, 4)
	-- int 型っぽくする。 ( time 表示 )
	local itimer = math.floor((g_timer-(GetNowCount()-(g_waitTime)))/1000)
	DrawString(WINDOW_W - 200, 16*2 + 4, "TIME       : " .. itimer, GetColor(200, 200, 255), 1)
	return true
end


-- プレイヤーにダメージが！！！
function ListenerPlayerDamage()
Blend.push()
Blend.set(Blend.ADD, 127)
	DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(255, 0, 0), TRUE)
	print("DAMEGE!!!")
Blend.pop()
end


-- 登録処理。
AddListener("scene-init", ListenerInit)
AddListener("scene-finish", ListenerFinish)
AddListener("scene-update", Update)
AddListener("scene-draw", Draw)
AddListener("change-bpm", ListenerChangeBpm)
AddListener("player-damage", ListenerPlayerDamage)


print("Load has Succeed.")

