-- -------------------------------------
-- Uzura
-- リロード
-- 2012 / 08 / 27
-- jskny
-- -------------------------------------


print("Uzura reload start.")


-- マトリックスシリアライズ用。
function SerializeMatrix(a)
	print(a._11 .. "," .. a._12 .. "," .. a._13 .. "," .. a._14)
	print(a._21 .. "," .. a._22 .. "," .. a._23 .. "," .. a._24)
	print(a._31 .. "," .. a._32 .. "," .. a._33 .. "," .. a._34)
	print(a._41 .. "," .. a._42 .. "," .. a._43 .. "," .. a._44)
end


-- イベントリスナー消去。
DelListenerAll()


print("Uzura reload finish.")

