SuperStrict

Framework bah.libsodium
Import brl.standardio


Local message:String = "Arbitrary data to hash"

Local hash:Byte[TGenericHash.Bytes()]


' without key
Print "~nwithout key.."

TGenericHash.Hash(hash, hash.length, message)

Print "~nhash = " + TUtil.Bin2Hex(hash, hash.length)


' with key
Print "~nwith key.."

Local key:Byte[TGenericHash.KeyBytes()]

TRandomBytes.Buffer(key, key.length)

Print "~nkey  = " + TUtil.Bin2Hex(key, key.length)


TGenericHash.Hash(hash, hash.length, message, key, key.length)

Print "~nhash = " + TUtil.Bin2Hex(hash, hash.length)

