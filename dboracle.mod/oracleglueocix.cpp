
#include "oci.h"
#include <exception>
#include <string>

class MaxSQLException;
class MaxEnvironment;
class MaxConnection;

using namespace std;

extern "C" {

#include "blitz.h"

	BBObject * _bah_dboracle_TOracleSQLException__create(int errorCode, BBString * message);

	MaxEnvironment * bmx_ora_environment_createEnvironment();
	MaxConnection * bmx_ora_environment_createConnection(MaxEnvironment * envHandle, BBString * user, BBString * pass, BBString * connect);



}

// ****************************************************

BBString * bmx_stringToBBString(string s) {
	return bbStringFromCString(s.c_str());
}

// ****************************************************

class MaxSQLException: public exception
{
public:
	MaxSQLException(int err, string message)
		: err(err), message(message)
	{
	}
	
	~MaxSQLException() throw()
	{
	}
	
	int getErrorCode() {
		return err;
	}
	
	string getMessage() {
		return message;
	}
	
private:
	int err;
	string message;
};

// **********************************

class MaxEnvironment
{
public:
	MaxEnvironment()
	{
	}
	
	~MaxEnvironment()
	{
		if (errHandle) {
			(void) OCIHandleFree((dvoid *) errHandle, OCI_HTYPE_ERROR);
		}
	
		if (envHandle) {
			(void) OCIHandleFree((dvoid *) envHandle, OCI_HTYPE_ENV);
		}
	}
	
	int Create() {
		int err = OCIEnvCreate((OCIEnv **) &envHandle, (ub4) OCI_DEFAULT,

                  (dvoid *) 0, (dvoid * (*)(dvoid *,size_t)) 0,

                  (dvoid * (*)(dvoid *, dvoid *, size_t)) 0,

                  (void (*)(dvoid *, dvoid *)) 0, (size_t) 0, (dvoid **) 0);

		if (err == 0) {
			(void) OCIHandleAlloc( (dvoid *) envHandle, (dvoid **) &errHandle, OCI_HTYPE_ERROR,

                   (size_t) 0, (dvoid **) 0);
		}

		return err;
	}

	static MaxEnvironment * createEnvironment() {
		MaxEnvironment * env = new MaxEnvironment();
		int err = env->Create();
		if (err != 0) {
			delete env;
			throw MaxSQLException(err, "OCIEnvCreate failed");
		}
		return env;
	}
	
	
private:
	OCIEnv * envHandle;
	OCIError * errHandle;
};


class MaxConnection
{
public:

private:

};


// ************************************

MaxEnvironment * bmx_ora_environment_createEnvironment() {
	MaxEnvironment * env = 0;
	
	try {
		env = MaxEnvironment::createEnvironment();
	} catch (MaxSQLException sqlEx) {
		bbExThrow(_bah_dboracle_TOracleSQLException__create(sqlEx.getErrorCode(), bmx_stringToBBString(sqlEx.getMessage())));
	}
	
	return env;
}

MaxConnection * bmx_ora_environment_createConnection(MaxEnvironment * envHandle, BBString * user, BBString * pass, BBString * connect) {



}


