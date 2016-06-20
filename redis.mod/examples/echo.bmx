SuperStrict

Framework BaH.Redis
Import brl.standardio


Local redis:TRedisClient = TRedisClient.Create()

Local reply:TRedisReply = redis.ECHO_("Hello World!")

Print reply.GetString()
