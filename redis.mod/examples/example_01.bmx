SuperStrict

Framework BaH.Redis
Import brl.standardio


Local redis:TRedisClient

Try
	redis = TRedisClient.Create()
Catch e:TRedisError
	Print e.ToString()
	End
End Try


Print redis.PING_().ToString()
Print redis.INFO_().ToString()
redis.ZADD_("someSet", 1, "one")
redis.ZADD_("someSet", 2, "two")

Print redis.ZRANGE_("someSet", 0, -1).ToString()

Print redis.PING_().ToString()

