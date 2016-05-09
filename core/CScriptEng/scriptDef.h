#pragma once
#include "notstd/notstd.h"

namespace runtime
{
	class runtimeContext;
	class FunctionObject;

	enum
	{
		// ��ջ����2��Ԫ�ص����������м��㣬������õ��Ľ��ѹ���ջ
		VM_add = 1,
		VM_sub,
		VM_mul,
		VM_div,
		VM_mod,
		VM_logicAnd,
		VM_logicOr,
		VM_logicNot,
		VM_bitwiseAnd,
		VM_bitwiseOr,
		VM_bitwiseXOR,
		VM_bitwiseNot,
		VM_setVal,

		// ���Ƕ�ջ�ϵ�����Ԫ�ؽ��бȽϵ�ָ�ͬʱ��������Ԫ�س�ջ
		// ����ȽϽ����������������ջ��ѹ������1������ѹ������0
		VM_equal,
		VM_notEqual,
		VM_greater,
		VM_less,
		VM_greaterEqual,
		VM_lessEqual,
		// ����ջ��Ԫ���Ƿ�Ϊ0�����Ϊ0������ת��ָ����λ��ִ��
		// ��ָ����Ҫ�����4�ֽ����ݣ����ڱ�־��ת��λ��
		// �������е���ֵ���ͣ���ָ�������ж���ֵ�Ƿ�Ϊ0����������������
		// �ú������ǲ���ת����������Ϊ��ֵ��Ϊ0��
		VM_jz,
		// ��������ת��ָ��λ�ã���ָ��Ҳ��Ҫ�����4���ֽ�����ָʾĿ��λ��
		VM_jump,

		// ���������4�ֽڣ����ڱ�ʾget��Ա�����ڳ������е�����
		VM_getMember,

		VM_doCall,
		VM_getIndex,

		// ������������Ԫ�ص�ָ��
		// ���������ݱ�ѹ��ջ
		// ����ָ��ĵڶ����ֽڱ�ʾ�������ǳ������Ǳ���
		// ����VM_createDouble�����8���ֽڣ�������涼��4���ֽڱ�ʾ����������ֵ
		VM_createchar,
		VM_createByte,
		VM_createShort,
		VM_createUshort,
		VM_createInt,
		VM_createUint,
		VM_createFloat,
		VM_createDouble,
		VM_createString,
		VM_createArray,

		// �����뵱ǰջ��ָ��ƫ�Ƶ������ٴ�ѹ��ջ����ƫ����Instrction��data�ṩ��
		// �ʲ��ܳ���65535��
		VM_push,
		VM_pop,

		// ����ջ֡��־��push
		// ��ʱInstruction��data��ָ�������ϵ��Ƶ�ջ֡����������0��
		// �����һ��uint32_t����ָʾ���ڸ�ջ֡��Ԫ��ƫ��
		// �����ڼ�ӵ�ַ����
		VM_copyAtFrame,

		// ջ֡��ά���������65536*256��ջ֡��Ŀǰ�����������֧���ӹ��̵���
		// ��֧�ֶ���Ĺ��ܵ��á����ǣ�����֧���ӹ��̵��õĻ���Ҳ��ʹ��ջ֡��
		// ά������ջ�����ԣ�����ֻ֧����ô�༶�ĵ���
		VM_pushStackFrame,
		VM_popStackFrame,

		VM_end,

		// data��ָʾ����������������������65535��
		VM_setParamCount,

		// ����ָ��1��ռ��һ��ָ���4�ֽڣ�+�����4�ֽڱ���
		VM_debug1,

		// Ϊ�˷��������룬���Ǽ����˱�ָ����ں�VM_jz���
		VM_jnz,

		// Ϊ�˷�����ԣ����ӱ�ָ������˳�Ӧ�ó����ִ��
		VM_debugbreak,

		// �����������󣬴�ʱ��4�ֽڵĶ�������ָ���˺��������˵������
		// ˵��������һ�����������˵���ṹ��struct FunctionDesc
		// ���ú���ʱ���ᴦ��ջ֡�����Ӻ�������ʱҲ���ջ֡�е������Ԫ��
		// �ָ�֮ǰ��ִ��ջ
		VM_createFunction,

		// �Ӻ������÷���
		VM_return,
	};

#pragma pack(push,1)
	struct Instruction
	{
		uint32_t code : 8;
		uint32_t extCode : 8;
		uint32_t data : 16;
	};

	struct FunctionDesc
	{
		// �������ֽڳ��ȣ�������4��������
		uint32_t len;
		// ����������id�����Ϊ0����ʾ�����������������
		uint32_t stringId;

		// �����Ĳ�������
		uint32_t paramCount;
	};
#pragma pack(pop)
	typedef uint32_t CommonInstruction;
}

namespace scriptAPI { class ScriptCompiler; }

namespace compiler
{
	class ConstStringData
	{
		// �����ַ�������ÿһ���һ��������ָʾ���������е����
		// �������ڼ��س���ʱ���롣������
		struct StringIndex
		{
			// �ַ����ڳ��������е�ƫ��
			uint32_t offset;
			// �ַ����ĳ��ȣ�������0��β�ַ���
			uint32_t size;
		};
		// ������
		std::vector<StringIndex> mConstStringIndexTable;
		// ����������ȡ��һ���ַ����������������������ȷ���Ƿ��Ѿ����ڸ��ַ���
		std::map<std::string,size_t> mIndexTable;

		// ��������ַ����ݵ��ڴ��
		std::string mStringBuffer;

	public:
		ConstStringData();
		void Clear();
		size_t RegistString(const std::string &str);
		bool GetString(size_t i, std::string &s) const;

		int SaveConstStringDataToFile(FILE *file) const;
		int LoadConstStringDataFromFile(FILE *file);
	};
	
	class GenerateInstructionHelper;
	class CompileResult
	{
		friend class GenerateInstructionHelper;
		friend class runtime::runtimeContext;
		friend class scriptAPI::ScriptCompiler;
		friend class runtime::FunctionObject;

	public:
		typedef std::vector<uint32_t> ScriptCode;

	private:
		// �������򣬴����Сһ����4�ֽڵ�������������ʱ����4�ֽڻ����)	
		ScriptCode mCode;

		ConstStringData *mConstStringData;

	private:
		inline ScriptCode& GetCode() { return mCode; }
		inline ConstStringData* GetStringData() { return mConstStringData; }
		// �õ���ǰ�Ĵ���ĩβλ��
		void Clear();

	public:
		CompileResult(ConstStringData *stringData);
		~CompileResult();

		uint32_t SaveCurrentCodePosition() const { return mCode.size(); }

		inline int SaveConstStringDataToFile(FILE *file) const
		{
			return mConstStringData->SaveConstStringDataToFile(file);
		}
		inline int LoadConstStringDataFromFile(FILE *file)
		{
			return mConstStringData->LoadConstStringDataFromFile(file);
		}
		
		int SaveCodeToFile(FILE *file) const;
		int LoadCodeFromFile(FILE *file);
	};

	class GenerateInstructionHelper
	{
	private:
		CompileResult *mCompileResult;
		compiler::CompileResult::ScriptCode &mCode;

	public:
		GenerateInstructionHelper(CompileResult *cr)
			: mCompileResult(cr)
			, mCode(cr->mCode)
		{
		}

		inline void SetCode(uint32_t pos, uint32_t val)
		{
			mCode[pos] = val;
		}

		inline uint32_t RegistName(const char *name)
		{
			return mCompileResult->GetStringData()->RegistString(name);
		}

		// ֱ���ڴ����в����ַ���������first�ǲ����λ�ã�second�ǻ���uint32_t�ĳ���
		// ��Щ�ɼ����ַ����ڲ鿴����ʱ�����׶�λ
		std::pair<uint32_t,uint32> InsertStringDataToCode(const char *str)
		{
			uint32_t l = (uint32_t)strlen(str);
			uint32_t actInsertLen = (l + 3) / 4;
			uint32_t sNow = mCode.size();
			mCode.resize(sNow + actInsertLen);
			memcpy(&mCode[sNow], str, l + 1);
			return std::make_pair(sNow, actInsertLen);
		}

		void Insert_add_Instruction()
		{
			mCode.push_back(runtime::VM_add);
		}

		void Insert_sub_Instruction()
		{
			mCode.push_back(runtime::VM_sub);
		}

		void Insert_mul_Instruction()
		{
			mCode.push_back(runtime::VM_mul);
		}

		void Insert_div_Instruction()
		{
			mCode.push_back(runtime::VM_div);
		}

		void Insert_mod_Instruction()
		{
			mCode.push_back(runtime::VM_mod);
		}

		void Insert_logicAnd_Instruction()
		{
			mCode.push_back(runtime::VM_logicAnd);
		}

		void Insert_logicOr_Instruction()
		{
			mCode.push_back(runtime::VM_logicOr);
		}

		void Insert_logicNot_Instruction()
		{
			mCode.push_back(runtime::VM_logicNot);
		}

		void Insert_bitwiseAnd_Instruction()
		{
			mCode.push_back(runtime::VM_bitwiseAnd);
		}

		void Insert_bitwiseOr_Instruction()
		{
			mCode.push_back(runtime::VM_bitwiseOr);
		}

		void Insert_bitwiseXOR_Instruction()
		{
			mCode.push_back(runtime::VM_bitwiseXOR);
		}

		void Insert_bitwiseNot_Instruction()
		{
			mCode.push_back(runtime::VM_bitwiseNot);
		}

		void Insert_setVal_Instruction()
		{
			mCode.push_back(runtime::VM_setVal);
		}

		void Insert_equal_Instruction()
		{
			mCode.push_back(runtime::VM_equal);
		}

		void Insert_notEqual_Instruction()
		{
			mCode.push_back(runtime::VM_notEqual);
		}

		void Insert_greater_Instruction()
		{
			mCode.push_back(runtime::VM_greater);
		}

		void Insert_less_Instruction()
		{
			mCode.push_back(runtime::VM_less);
		}

		void Insert_greaterEqual_Instruction()
		{
			mCode.push_back(runtime::VM_greaterEqual);
		}

		void Insert_lessEqual_Instruction()
		{
			mCode.push_back(runtime::VM_lessEqual);
		}

		// ��������תָ����뺯���Ĳ�������תĿ�꣬�����ȷ������д�κ���
		// ���ǵķ���ֵ����תĿ���ַ��ָ��ƫ��λ��
		uint32_t Insert_jz_Instruction(uint32_t target)
		{
			mCode.push_back(runtime::VM_jz);
			uint32_t r = mCompileResult->SaveCurrentCodePosition();
			mCode.push_back(target);
			return r;
		}

		uint32_t Insert_jump_Instruction(uint32_t target)
		{
			mCode.push_back(runtime::VM_jump);
			uint32_t r = mCompileResult->SaveCurrentCodePosition();
			mCode.push_back(target);
			return r;
		}

		void Insert_getMember_Instruction(const char *name)
		{
			mCode.push_back(runtime::VM_getMember);
			mCode.push_back(
				static_cast<uint32_t>(
				mCompileResult->GetStringData()->RegistString(name)
				));
		}

		void Insert_doCall_Instruction()
		{
			mCode.push_back(runtime::VM_doCall);
		}

		void Insert_getIndex_Instruction()
		{
			mCode.push_back(runtime::VM_getIndex);
		}

		void Insert_createChar_Instruction(uint32_t v)
		{
			mCode.push_back(runtime::VM_createchar);
			mCode.push_back(v);
		}

		void Insert_createByte_Instruction(uint32_t v)
		{
			mCode.push_back(runtime::VM_createByte);
			mCode.push_back(v);
		}

		void Insert_createShort_Instruction(uint32_t v)
		{
			mCode.push_back(runtime::VM_createShort);
			mCode.push_back(v);
		}

		void Insert_createUshort_Instruction(uint32_t v)
		{
			mCode.push_back(runtime::VM_createUshort);
			mCode.push_back(v);
		}

		void Insert_createInt_Instruction(int v)
		{
			mCode.push_back(runtime::VM_createInt);
			mCode.push_back(*reinterpret_cast<uint32_t*>(&v));
		}

		void Insert_createUint_Instruction(uint32_t v)
		{
			mCode.push_back(runtime::VM_createUint);
			mCode.push_back(v);
		}

		void Insert_createFloat_Instruction(float v)
		{
			mCode.push_back(runtime::VM_createFloat);
			mCode.push_back(*reinterpret_cast<uint32_t*>(&v));
		}

		void Insert_createDouble_Instruction(double v)
		{
			// ����ָ��������������Ҳû���õ�����ռ��λ�ð�
			assert(0);
			mCode.push_back(runtime::VM_createDouble);
			mCode.resize(mCode.size() + 2);
			*reinterpret_cast<double*>(&mCode[mCode.size() - 2]) = v;
		}

		void Insert_createString_Instruction(const char *name)
		{
			mCode.push_back(runtime::VM_createString);
			mCode.push_back(
				static_cast<uint32_t>(
				mCompileResult->GetStringData()->RegistString(name)
				));
		}

		void Insert_createArray_Instruction()
		{
			mCode.push_back(runtime::VM_createArray);
			mCode.push_back(0);
		}

		void Insert_push_Instruction(uint16_t off)
		{
			runtime::Instruction inst;
			inst.code = runtime::VM_push;
			inst.data = off;
			mCode.push_back(*reinterpret_cast<uint32_t*>(&inst));
		}

		void Insert_pop_Instruction()
		{
			mCode.push_back(runtime::VM_pop);
		}

		void Insert_copyAtFrame_Instruction(uint16_t frameOff, uint32_t index)
		{
			runtime::Instruction inst;
			inst.code = runtime::VM_copyAtFrame;
			inst.data = frameOff;
			mCode.push_back(*reinterpret_cast<uint32_t*>(&inst));
			mCode.push_back(index);
		}

		void Insert_pushStackFrame_Instruction()
		{
			mCode.push_back(runtime::VM_pushStackFrame);
		}
		
		void Insert_popStackFrame_Instruction()
		{
			mCode.push_back(runtime::VM_popStackFrame);
		}

		void Insert_end_Instruction()
		{
			mCode.push_back(runtime::VM_end);
		}

		void Insert_setParamCount_Instruction(uint16_t paramCount)
		{
			runtime::Instruction inst;
			inst.code = runtime::VM_setParamCount;
			inst.data = paramCount;
			mCode.push_back(*reinterpret_cast<uint32_t*>(&inst));
		}

		void Insert_debug1_Instruction(uint32_t param = 0)
		{
			mCode.push_back(runtime::VM_debug1);
			mCode.push_back(param);
		}

		uint32_t Insert_jnz_Instruction(uint32_t target)
		{
			mCode.push_back(runtime::VM_jnz);
			uint32_t r = mCompileResult->SaveCurrentCodePosition();
			mCode.push_back(target);
			return r;
		}

		void Insert_debugbreak_Instruction(uint32_t param = 0)
		{
			mCode.push_back(runtime::VM_debugbreak);
			mCode.push_back(param);
		}

		uint32_t Insert_createFunction_Instruction()
		{
			mCode.push_back(runtime::VM_createFunction);
			uint32_t r = mCompileResult->SaveCurrentCodePosition();
			mCode.push_back(0);
			return r;
		}

		void InsertFunctionDesc(runtime::FunctionDesc *funcDesc)
		{
			uint32_t s = mCode.size();
			mCode.resize(s + sizeof(runtime::FunctionDesc) / 4);
			*reinterpret_cast<runtime::FunctionDesc*>(&mCode[s]) = *funcDesc;
		}

		void Insert_return_Instruction()
		{
			mCode.push_back(runtime::VM_return);
		}
	};
}
