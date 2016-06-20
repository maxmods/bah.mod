SuperStrict

Framework BaH.Redis
Import brl.standardio


Local redis:TRedisClient = TRedisClient.Create()

Local reply:TRedisReply = redis.PING_()

Print reply.GetString()


