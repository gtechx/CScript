/*
	copyright mz02005@qq.com
	���ű��ļ������ڱ�����ɺ󣬽����������������ͷ�ļ������ļ��ȸ��Ƶ��ض���Ŀ¼�����㷢��
*/

// ��һ��������$(SolutionDir)
// �ڶ�����������ϵ�ṹ
// ������������debug����release
if (ARGV.size <= 2)
{
	println("Invalid parameter, solution dir must be identified");
	return 1;
}

string rootPath = ARGV[0];
if (rootPath.substr(rootPath.len - 1) != "\\")
	rootPath += "\\";

string devReleaseRoot = rootPath + "devRelease\\";

string arch = ARGV[1];
string conf = ARGV[2];

function mkdirHelper(dir)
{
	println("mkdir " + dir);
	system("mkdir " + dir);
	return 0;
}

function forEachInArray(arr, f)
{
	int i;
	int s = arr.size;
	for (i = 0; i < s; i += 1)
		f(i, arr[i]);
}

// ����Ŀ¼
array dirToCreate = CreateArray(
	devReleaseRoot,
	devReleaseRoot + "include",
	devReleaseRoot + "include\\notstd",

	devReleaseRoot + "lib",
	devReleaseRoot + "bin",

	devReleaseRoot + "lib\\" + arch,
	devReleaseRoot + "lib\\" + arch + "\\" + conf,

	devReleaseRoot + "bin\\" + arch,
	devReleaseRoot + "bin\\" + arch + "\\" + conf,
	);

forEachInArray(dirToCreate, function(i, e)
{
	mkdirHelper(e);
});

// �����Ƶ�ͷ�ļ�
array incToCopy = CreateArray(
	"CScriptEng.h",
	"rtTypes.h",
	"objType.h",
	"arrayType.h",
	"cscriptBase.h",
	"config.h"
	);

function CopyFile(src, dest)
{
	println("copy " + src + " to " + dest);
	system("copy " + src + " " + dest);
}

forEachInArray(incToCopy, function(i, e)
{
	string destDir = devReleaseRoot + "include\\";
	string src = rootPath + "core\\CScriptEng\\" + e;
	CopyFile(src, destDir);
});

// notstd��һ��������ļ�Ҫ����
CopyFile(rootPath + "core\\notstd\\config.h", devReleaseRoot + "include\\notstd");

string targetDir = rootPath + arch + "\\" + conf + "\\";

// ���ƿ��ļ�
CopyFile(targetDir + "*.lib", devReleaseRoot + "lib\\" + arch + "\\" + conf);

// ���ƶ�̬��
CopyFile(targetDir + "*.dll", devReleaseRoot + "bin\\" + arch + "\\" + conf);

//zipFile(rootPath + "devRelease.zip", devReleaseRoot, CreateArray(1));

// ����windows��shell�˵������ڷ����ִ�нű��ļ�
array shellMenuToWrite = 
	CreateArray(
		"Windows Registry Editor Version 5.00\r\n",
		"[HKEY_CLASSES_ROOT\\*\\shell\\��cscripteng��\\command]",
		"@=\"" + rootPath + "win32\\debug\\testCScript.exe \\\"%1\\\"\"" + "\r\n");

string regFilePath = rootPath + "temp.reg";
DeleteFile(regFilePath);
object rf = csOpenFile(regFilePath, "wb");
if (rf != null)
{
	forEachInArray(shellMenuToWrite, function(i, e)
	{
		rf.WriteFile(e + "\r\n");
	});
	rf.Close();
	system("regedit " + regFilePath);
}
//DeleteFile(regFilePath);
