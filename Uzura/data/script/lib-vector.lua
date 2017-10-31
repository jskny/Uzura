-- -------------------------------------
-- Uzura
-- Vector 演算用関数群
-- 2012 / 08 / 28
-- jskny
-- -------------------------------------


-- -------------------------------------
-- Vector オブジェクト
-- -------------------------------------


Vector = {}


function Vector.get(x, y, z)
	return setmetatable(obj, { __index = Vector })
end


function Vector.serialize(self)
	local str = (self.x .. "," .. self.y .. "," .. self.z)
	return str
end


function Vector.new()
	local obj = { x = 0.0, y = 0.0, z = 0.0 }
	return setmetatable(obj, { __index = Vector })
end


-- -------------------------------------


-- 取得
function VGet(x, y, z)
	local t = Vector.new()
	t.x = x;
	t.y = y;
	t.z = z;

	return t
end


-- 加算
function VAdd(a, b)
	local t = Vector.new()
	t.x = a.x + b.x;
	t.y = a.y + b.y;
	t.z = a.z + b.z;

	return t
end


-- 減算
function VSub(a, b)
	local t = Vector.new()
	t.x = a.x - b.x;
	t.y = a.y - b.y;
	t.z = a.z - b.z;

	return t
end


-- 内積を取得
function VDot(a, b)
	local t = (a.x * b.x) + (a.y * b.y) + (a.z * b.z)
	return t
end


-- 外積を取得
function VCross(a, b)
	local t = Vector.new()
	t.x = a.y * b.z - a.z * b.y;
	t.y = a.z * b.x - a.x * b.z;
	t.z = a.x * b.y - a.y * b.x;

	return t
end


-- ベクトルのサイズを取得
function VSize(a)
	return (math.sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z)))
end

