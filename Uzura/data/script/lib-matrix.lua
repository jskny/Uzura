-- -------------------------------------
-- Uzura
-- Matrix 演算用関数群
-- 2012 / 09 / 18
-- jskny
-- -------------------------------------


-- -------------------------------------
-- Matrix オブジェクト
-- -------------------------------------


Matrix = {}


function Matrix.get()
	local obj = {
		_11 = 0, _12 = 0, _13 = 0, _14 = 0,
		_21 = 0, _22 = 0, _23 = 0, _24 = 0,
		_31 = 0, _32 = 0, _33 = 0, _34 = 0,
		_41 = 0, _42 = 0, _43 = 0, _44 = 0
	}


	return setmetatable(obj, { __index = Matrix })
end


-- マトリックスシリアライズ用。
function Matrix.serialize(self)
	local str = ""
	str = str .. (self._11 .. "," .. self._12 .. "," .. self._13 .. "," .. self._14) .. "\n"
	str = str .. (self._21 .. "," .. self._22 .. "," .. self._23 .. "," .. self._24) .. "\n"
	str = str .. (self._31 .. "," .. self._32 .. "," .. self._33 .. "," .. self._34) .. "\n"
	str = str .. (self._41 .. "," .. self._42 .. "," .. self._43 .. "," .. self._44) .. "\n"

	return (str)
end


function Matrix.new()
	local obj = {
		_11 = 0, _12 = 0, _13 = 0, _14 = 0,
		_21 = 0, _22 = 0, _23 = 0, _24 = 0,
		_31 = 0, _32 = 0, _33 = 0, _34 = 0,
		_41 = 0, _42 = 0, _43 = 0, _44 = 0
	}


	return setmetatable(obj, { __index = Matrix })
end


-- 単位行列の取得
function Matrix.getIdent()
	local obj = {
		_11 = 1, _12 = 0, _13 = 0, _14 = 0,
		_21 = 0, _22 = 1, _23 = 0, _24 = 0,
		_31 = 0, _32 = 0, _33 = 1, _34 = 0,
		_41 = 0, _42 = 0, _43 = 0, _44 = 1
	}


	return setmetatable(obj, { __index = Matrix })
end


-- 拡大縮小行列の取得
function Matrix.getScale(s)
	local obj = {
		_11 = s, _12 = 0, _13 = 0, _14 = 0,
		_21 = 0, _22 = s, _23 = 0, _24 = 0,
		_31 = 0, _32 = 0, _33 = s, _34 = 0,
		_41 = 0, _42 = 0, _43 = 0, _44 = 1
	}


	return setmetatable(obj, { __index = Matrix })
end


-- 並行移動行列
function Matrix.getTranslate(v)
	local obj = {
		_11 = 0, _12 = 0, _13 = 0, _14 = 0,
		_21 = 0, _22 = 0, _23 = 0, _24 = 0,
		_31 = 0, _32 = 0, _33 = 0, _34 = 0,
		_41 = v.x, _42 = v.y, _43 = v.z, _44 = 1
	}


	return setmetatable(obj, { __index = Matrix })
end


-- X軸回転行列
function Matrix.getRotX(XAxisRotate)
	local s = math.sin(XAxisRotate);
	local c = math.cos(XAxisRotate);


	local obj = {
		_11 = 1, _12 = 0, _13 = 0, _14 = 0,
		_21 = 0, _22 = c, _23 = s, _24 = 0,
		_31 = 0, _32 = -s, _33 = c, _34 = 0,
		_41 = 0, _42 = 0, _43 = 0, _44 = 1
	}


	return setmetatable(obj, { __index = Matrix })
end


-- Y軸回転行列
function Matrix.getRotY(YAxisRotate)
	local s = math.sin(YAxisRotate);
	local c = math.cos(YAxisRotate);


	local obj = {
		_11 = c, _12 = 0, _13 = s, _14 = 0,
		_21 = 0, _22 = 1, _23 = 0, _24 = 0,
		_31 = s, _32 = 0, _33 = c, _34 = 0,
		_41 = 0, _42 = 0, _43 = 0, _44 = 1
	}


	return setmetatable(obj, { __index = Matrix })
end


-- Z軸回転行列
function Matrix.getRotZ(ZAxisRotate)
	local s = math.sin(ZAxisRotate);
	local c = math.cos(ZAxisRotate);


	local obj = {
		_11 = c, _12 = s, _13 = 0, _14 = 0,
		_21 = -s, _22 = c, _23 = 0, _24 = 0,
		_31 = s, _32 = 0, _33 = 1, _34 = 0,
		_41 = 0, _42 = 0, _43 = 0, _44 = 1
	}


	return setmetatable(obj, { __index = Matrix })
end


-- 行列の加算
function Matrix.add(a, b)
	local c = Matrix.new()
	c._11 = a._11 + b._11
	c._12 = a._12 + b._12
	c._13 = a._13 + b._13
	c._14 = a._14 + b._14

	c._21 = a._21 + b._21
	c._22 = a._22 + b._22
	c._23 = a._23 + b._23
	c._24 = a._24 + b._24

	c._31 = a._31 + b._31
	c._32 = a._32 + b._32
	c._33 = a._33 + b._33
	c._34 = a._34 + b._34

	c._41 = a._41 + b._41
	c._42 = a._42 + b._42
	c._43 = a._43 + b._43
	c._44 = a._44 + b._44
end


