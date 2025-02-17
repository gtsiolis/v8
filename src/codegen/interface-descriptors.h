// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_CODEGEN_INTERFACE_DESCRIPTORS_H_
#define V8_CODEGEN_INTERFACE_DESCRIPTORS_H_

#include <memory>

#include "src/codegen/machine-type.h"
#include "src/codegen/register-arch.h"
#include "src/codegen/tnode.h"
#include "src/common/globals.h"
#include "src/execution/isolate.h"

namespace v8 {
namespace internal {

#define TORQUE_BUILTIN_LIST_TFC(V)                                            \
  BUILTIN_LIST_FROM_TORQUE(IGNORE_BUILTIN, IGNORE_BUILTIN, V, IGNORE_BUILTIN, \
                           IGNORE_BUILTIN, IGNORE_BUILTIN)

#define INTERFACE_DESCRIPTOR_LIST(V)     \
  V(Abort)                               \
  V(Allocate)                            \
  V(ApiCallback)                         \
  V(ApiGetter)                           \
  V(ArrayConstructor)                    \
  V(ArrayNArgumentsConstructor)          \
  V(ArrayNoArgumentConstructor)          \
  V(ArraySingleArgumentConstructor)      \
  V(AsyncFunctionStackParameter)         \
  V(BigIntToI32Pair)                     \
  V(BigIntToI64)                         \
  V(BinaryOp)                            \
  V(BinaryOp_Baseline)                   \
  V(BinaryOp_WithFeedback)               \
  V(CallForwardVarargs)                  \
  V(CallFunctionTemplate)                \
  V(CallTrampoline)                      \
  V(CallTrampoline_Baseline)             \
  V(CallTrampoline_WithFeedback)         \
  V(CallVarargs)                         \
  V(CallWithArrayLike)                   \
  V(CallWithArrayLike_WithFeedback)      \
  V(CallWithSpread)                      \
  V(CallWithSpread_Baseline)             \
  V(CallWithSpread_WithFeedback)         \
  V(CEntry1ArgvOnStack)                  \
  V(CloneObjectBaseline)                 \
  V(CloneObjectWithVector)               \
  V(Compare)                             \
  V(Compare_Baseline)                    \
  V(Compare_WithFeedback)                \
  V(ConstructForwardVarargs)             \
  V(ConstructStub)                       \
  V(ConstructVarargs)                    \
  V(ConstructWithArrayLike)              \
  V(ConstructWithArrayLike_WithFeedback) \
  V(Construct_WithFeedback)              \
  V(Construct_Baseline)                  \
  V(ConstructWithSpread)                 \
  V(ConstructWithSpread_Baseline)        \
  V(ConstructWithSpread_WithFeedback)    \
  V(ContextOnly)                         \
  V(CppBuiltinAdaptor)                   \
  V(DynamicCheckMaps)                    \
  V(EphemeronKeyBarrier)                 \
  V(FastNewObject)                       \
  V(ForInPrepare)                        \
  V(FrameDropperTrampoline)              \
  V(GetIteratorStackParameter)           \
  V(GetProperty)                         \
  V(GrowArrayElements)                   \
  V(I32PairToBigInt)                     \
  V(I64ToBigInt)                         \
  V(InterpreterCEntry1)                  \
  V(InterpreterCEntry2)                  \
  V(InterpreterDispatch)                 \
  V(TailCallOptimizedCodeSlot)           \
  V(InterpreterPushArgsThenCall)         \
  V(InterpreterPushArgsThenConstruct)    \
  V(JSTrampoline)                        \
  V(BaselineOutOfLinePrologue)           \
  V(BaselineLeaveFrame)                  \
  V(Load)                                \
  V(LoadBaseline)                        \
  V(LoadGlobal)                          \
  V(LoadGlobalBaseline)                  \
  V(LoadGlobalNoFeedback)                \
  V(LoadGlobalWithVector)                \
  V(LoadNoFeedback)                      \
  V(LoadWithVector)                      \
  V(LoadWithReceiverAndVector)           \
  V(LoadWithReceiverBaseline)            \
  V(LookupBaseline)                      \
  V(NoContext)                           \
  V(RecordWrite)                         \
  V(ResumeGenerator)                     \
  V(RunMicrotasks)                       \
  V(RunMicrotasksEntry)                  \
  V(SingleParameterOnStack)              \
  V(Store)                               \
  V(StoreBaseline)                       \
  V(StoreGlobal)                         \
  V(StoreGlobalBaseline)                 \
  V(StoreGlobalWithVector)               \
  V(StoreTransition)                     \
  V(StoreWithVector)                     \
  V(StringAt)                            \
  V(StringAtAsString)                    \
  V(StringSubstring)                     \
  V(TypeConversion)                      \
  V(TypeConversionNoContext)             \
  V(TypeConversion_Baseline)             \
  V(Typeof)                              \
  V(UnaryOp_Baseline)                    \
  V(UnaryOp_WithFeedback)                \
  V(Void)                                \
  V(WasmFloat32ToNumber)                 \
  V(WasmFloat64ToNumber)                 \
  V(WasmI32AtomicWait32)                 \
  V(WasmI64AtomicWait32)                 \
  BUILTIN_LIST_TFS(V)                    \
  TORQUE_BUILTIN_LIST_TFC(V)

enum class StackArgumentOrder {
  kDefault,  // Arguments in the stack are pushed in the default/stub order (the
             // first argument is pushed first).
  kJS,  // Arguments in the stack are pushed in the same order as the one used
        // by JS-to-JS function calls. This should be used if calling a
        // JSFunction or if the builtin is expected to be called directly from a
        // JSFunction. This order is reversed compared to kDefault.
};

class V8_EXPORT_PRIVATE CallInterfaceDescriptorData {
 public:
  enum Flag {
    kNoFlags = 0u,
    kNoContext = 1u << 0,
    // This indicates that the code uses a special frame that does not scan the
    // stack arguments, e.g. EntryFrame. And this allows the code to use
    // untagged stack arguments.
    kNoStackScan = 1u << 1,
    // In addition to the specified parameters, additional arguments can be
    // passed on the stack.
    // This does not indicate if arguments adaption is used or not.
    kAllowVarArgs = 1u << 2,
  };
  using Flags = base::Flags<Flag>;

  CallInterfaceDescriptorData() = default;

  CallInterfaceDescriptorData(const CallInterfaceDescriptorData&) = delete;
  CallInterfaceDescriptorData& operator=(const CallInterfaceDescriptorData&) =
      delete;

  // A copy of the passed in registers and param_representations is made
  // and owned by the CallInterfaceDescriptorData.

  void InitializePlatformSpecific(int register_parameter_count,
                                  const Register* registers);

  // if machine_types is null, then an array of size
  // (return_count + parameter_count) will be created with
  // MachineType::AnyTagged() for each member.
  //
  // if machine_types is not null, then it should be of the size
  // (return_count + parameter_count). Those members of the parameter array will
  // be initialized from {machine_types}, and the rest initialized to
  // MachineType::AnyTagged().
  void InitializePlatformIndependent(Flags flags, int return_count,
                                     int parameter_count,
                                     const MachineType* machine_types,
                                     int machine_types_length,
                                     StackArgumentOrder stack_order);

  void Reset();

  bool IsInitialized() const {
    return IsInitializedPlatformSpecific() &&
           IsInitializedPlatformIndependent();
  }

  Flags flags() const { return flags_; }
  int return_count() const { return return_count_; }
  int param_count() const { return param_count_; }
  int register_param_count() const { return register_param_count_; }
  Register register_param(int index) const { return register_params_[index]; }
  Register* register_params() const { return register_params_; }
  MachineType return_type(int index) const {
    DCHECK_LT(index, return_count_);
    return machine_types_[index];
  }
  MachineType param_type(int index) const {
    DCHECK_LT(index, param_count_);
    return machine_types_[return_count_ + index];
  }
  StackArgumentOrder stack_order() const { return stack_order_; }

  void RestrictAllocatableRegisters(const Register* registers, int num) {
    DCHECK_EQ(allocatable_registers_, 0);
    for (int i = 0; i < num; ++i) {
      allocatable_registers_ |= registers[i].bit();
    }
    DCHECK_GT(NumRegs(allocatable_registers_), 0);
  }

  RegList allocatable_registers() const { return allocatable_registers_; }

 private:
  bool IsInitializedPlatformSpecific() const {
    const bool initialized =
        (register_param_count_ == 0 && register_params_ == nullptr) ||
        (register_param_count_ > 0 && register_params_ != nullptr);
    // Platform-specific initialization happens before platform-independent.
    return initialized;
  }
  bool IsInitializedPlatformIndependent() const {
    const bool initialized =
        return_count_ >= 0 && param_count_ >= 0 && machine_types_ != nullptr;
    // Platform-specific initialization happens before platform-independent.
    return initialized;
  }

#ifdef DEBUG
  bool AllStackParametersAreTagged() const;
#endif  // DEBUG

  int register_param_count_ = -1;
  int return_count_ = -1;
  int param_count_ = -1;
  Flags flags_ = kNoFlags;
  StackArgumentOrder stack_order_ = StackArgumentOrder::kDefault;

  // Specifying the set of registers that could be used by the register
  // allocator. Currently, it's only used by RecordWrite code stub.
  RegList allocatable_registers_ = 0;

  // |registers_params_| defines registers that are used for parameter passing.
  // |machine_types_| defines machine types for resulting values and incomping
  // parameters.
  // Both arrays are allocated dynamically by the InterfaceDescriptor and
  // freed on destruction. This is because static arrays cause creation of
  // runtime static initializers which we don't want.
  Register* register_params_ = nullptr;
  MachineType* machine_types_ = nullptr;
};

class V8_EXPORT_PRIVATE CallDescriptors : public AllStatic {
 public:
  enum Key {
#define DEF_ENUM(name, ...) name,
    INTERFACE_DESCRIPTOR_LIST(DEF_ENUM)
#undef DEF_ENUM
        NUMBER_OF_DESCRIPTORS
  };

  static void InitializeOncePerProcess();
  static void TearDown();

  static CallInterfaceDescriptorData* call_descriptor_data(
      CallDescriptors::Key key) {
    return &call_descriptor_data_[key];
  }

  static Key GetKey(const CallInterfaceDescriptorData* data) {
    ptrdiff_t index = data - call_descriptor_data_;
    DCHECK_LE(0, index);
    DCHECK_LT(index, CallDescriptors::NUMBER_OF_DESCRIPTORS);
    return static_cast<CallDescriptors::Key>(index);
  }

 private:
  static CallInterfaceDescriptorData
      call_descriptor_data_[NUMBER_OF_DESCRIPTORS];
};

class V8_EXPORT_PRIVATE CallInterfaceDescriptor {
 public:
  using Flags = CallInterfaceDescriptorData::Flags;

  CallInterfaceDescriptor() : data_(nullptr) {}
  virtual ~CallInterfaceDescriptor() = default;

  explicit CallInterfaceDescriptor(CallDescriptors::Key key)
      : data_(CallDescriptors::call_descriptor_data(key)) {}

  Flags flags() const { return data()->flags(); }

  bool HasContextParameter() const {
    return (flags() & CallInterfaceDescriptorData::kNoContext) == 0;
  }

  bool AllowVarArgs() const {
    return flags() & CallInterfaceDescriptorData::kAllowVarArgs;
  }

  int GetReturnCount() const { return data()->return_count(); }

  MachineType GetReturnType(int index) const {
    DCHECK_LT(index, data()->return_count());
    return data()->return_type(index);
  }

  int GetParameterCount() const { return data()->param_count(); }

  int GetRegisterParameterCount() const {
    return data()->register_param_count();
  }

  int GetStackParameterCount() const {
    return data()->param_count() - data()->register_param_count();
  }

  Register GetRegisterParameter(int index) const {
    DCHECK_LT(index, data()->register_param_count());
    return data()->register_param(index);
  }

  MachineType GetParameterType(int index) const {
    DCHECK_LT(index, data()->param_count());
    return data()->param_type(index);
  }

  RegList allocatable_registers() const {
    return data()->allocatable_registers();
  }

  StackArgumentOrder GetStackArgumentOrder() const {
    return data()->stack_order();
  }

  static const Register ContextRegister();

  const char* DebugName() const;

  bool operator==(const CallInterfaceDescriptor& other) const {
    return data() == other.data();
  }

 protected:
  const CallInterfaceDescriptorData* data() const { return data_; }

  virtual void InitializePlatformSpecific(CallInterfaceDescriptorData* data) {
    UNREACHABLE();
  }

  // Initializes |data| to an unspecified state, for platforms that haven't
  // implemented a given builtin.
  static void InitializePlatformUnimplemented(CallInterfaceDescriptorData* data,
                                              int register_parameter_count) {
    DefaultInitializePlatformSpecific(data,
                                      std::min(register_parameter_count, 4));
  }

  virtual void InitializePlatformIndependent(
      CallInterfaceDescriptorData* data) {
    // Default descriptor configuration: one result, all parameters are passed
    // in registers and all parameters have MachineType::AnyTagged() type.
    data->InitializePlatformIndependent(
        CallInterfaceDescriptorData::kNoFlags, 1, data->register_param_count(),
        nullptr, 0, StackArgumentOrder::kDefault);
  }

  // Initializes |data| using the platform dependent default set of registers.
  // It is intended to be used for TurboFan stubs when particular set of
  // registers does not matter.
  static void DefaultInitializePlatformSpecific(
      CallInterfaceDescriptorData* data, int register_parameter_count);

  // Initializes |data| using the platform dependent default set of registers
  // for JavaScript-compatible calling convention.
  // It is intended to be used for TurboFan stubs being called with JavaScript
  // linkage + additional parameters on registers and stack.
  static void JSDefaultInitializePlatformSpecific(
      CallInterfaceDescriptorData* data, int non_js_register_parameter_count);

  // Checks if float parameters are not assigned invalid registers.
  bool CheckFloatingPointParameters(CallInterfaceDescriptorData* data) {
    for (int i = 0; i < data->register_param_count(); i++) {
      if (IsFloatingPoint(data->param_type(i).representation())) {
        if (!IsValidFloatParameterRegister(data->register_param(i))) {
          return false;
        }
      }
    }
    return true;
  }

  bool IsValidFloatParameterRegister(Register reg);

 private:
  // {CallDescriptors} is allowed to call the private {Initialize} method.
  friend class CallDescriptors;

  const CallInterfaceDescriptorData* data_;

  void Initialize(CallInterfaceDescriptorData* data) {
    // The passed pointer should be a modifiable pointer to our own data.
    DCHECK_EQ(data, data_);
    DCHECK(!data->IsInitialized());
    InitializePlatformSpecific(data);
    InitializePlatformIndependent(data);
    DCHECK(data->IsInitialized());
    DCHECK(CheckFloatingPointParameters(data));
  }
};

#define DECLARE_DESCRIPTOR_WITH_BASE(name, base) \
 public:                                         \
  explicit name() : base(key()) {}               \
  static inline CallDescriptors::Key key();

#if defined(V8_TARGET_ARCH_IA32)
// To support all possible cases, we must limit the number of register args for
// TFS builtins on ia32 to 3. Out of the 6 allocatable registers, esi is taken
// as the context register and ebx is the root register. One register must
// remain available to store the jump/call target. Thus 3 registers remain for
// arguments. The reason this applies to TFS builtins specifically is because
// this becomes relevant for builtins used as targets of Torque function
// pointers (which must have a register available to store the target).
// TODO(jgruber): Ideally we should just decrement kMaxBuiltinRegisterParams but
// that comes with its own set of complications. It's possible, but requires
// refactoring the calling convention of other existing stubs.
constexpr int kMaxBuiltinRegisterParams = 4;
constexpr int kMaxTFSBuiltinRegisterParams = 3;
#else
constexpr int kMaxBuiltinRegisterParams = 5;
constexpr int kMaxTFSBuiltinRegisterParams = kMaxBuiltinRegisterParams;
#endif
STATIC_ASSERT(kMaxTFSBuiltinRegisterParams <= kMaxBuiltinRegisterParams);

#define DECLARE_DEFAULT_DESCRIPTOR(name, base)                                 \
  DECLARE_DESCRIPTOR_WITH_BASE(name, base)                                     \
 protected:                                                                    \
  static const int kRegisterParams =                                           \
      kParameterCount > kMaxTFSBuiltinRegisterParams                           \
          ? kMaxTFSBuiltinRegisterParams                                       \
          : kParameterCount;                                                   \
  static const int kStackParams = kParameterCount - kRegisterParams;           \
  void InitializePlatformSpecific(CallInterfaceDescriptorData* data)           \
      override {                                                               \
    DefaultInitializePlatformSpecific(data, kRegisterParams);                  \
  }                                                                            \
  void InitializePlatformIndependent(CallInterfaceDescriptorData* data)        \
      override {                                                               \
    data->InitializePlatformIndependent(Flags(kDescriptorFlags), kReturnCount, \
                                        kParameterCount, nullptr, 0,           \
                                        kStackArgumentOrder);                  \
  }                                                                            \
  name(CallDescriptors::Key key) : base(key) {}                                \
                                                                               \
 public:

#define DECLARE_JS_COMPATIBLE_DESCRIPTOR(name, base,                        \
                                         non_js_reg_parameters_count)       \
  DECLARE_DESCRIPTOR_WITH_BASE(name, base)                                  \
 protected:                                                                 \
  void InitializePlatformSpecific(CallInterfaceDescriptorData* data)        \
      override {                                                            \
    JSDefaultInitializePlatformSpecific(data, non_js_reg_parameters_count); \
  }                                                                         \
  name(CallDescriptors::Key key) : base(key) {}                             \
                                                                            \
 public:

#define DEFINE_FLAGS_AND_RESULT_AND_PARAMETERS(flags, stack_order,       \
                                               return_count, ...)        \
  static constexpr int kDescriptorFlags = flags;                         \
  static constexpr int kReturnCount = return_count;                      \
  static constexpr StackArgumentOrder kStackArgumentOrder = stack_order; \
  enum ParameterIndices {                                                \
    __dummy = -1, /* to be able to pass zero arguments */                \
    ##__VA_ARGS__,                                                       \
                                                                         \
    kParameterCount,                                                     \
    kContext = kParameterCount /* implicit parameter */                  \
  };

#define DEFINE_RESULT_AND_PARAMETERS(return_count, ...)                    \
  DEFINE_FLAGS_AND_RESULT_AND_PARAMETERS(                                  \
      CallInterfaceDescriptorData::kNoFlags, StackArgumentOrder::kDefault, \
      return_count, ##__VA_ARGS__)

// This is valid only for builtins that use EntryFrame, which does not scan
// stack arguments on GC.
#define DEFINE_PARAMETERS_ENTRY(...)                        \
  static constexpr int kDescriptorFlags =                   \
      CallInterfaceDescriptorData::kNoContext |             \
      CallInterfaceDescriptorData::kNoStackScan;            \
  static constexpr StackArgumentOrder kStackArgumentOrder = \
      StackArgumentOrder::kDefault;                         \
  static constexpr int kReturnCount = 1;                    \
  enum ParameterIndices {                                   \
    __dummy = -1, /* to be able to pass zero arguments */   \
    ##__VA_ARGS__,                                          \
                                                            \
    kParameterCount                                         \
  };

#define DEFINE_PARAMETERS(...)                                                \
  DEFINE_FLAGS_AND_RESULT_AND_PARAMETERS(                                     \
      CallInterfaceDescriptorData::kNoFlags, StackArgumentOrder::kDefault, 1, \
      ##__VA_ARGS__)

#define DEFINE_PARAMETERS_NO_CONTEXT(...)                                    \
  DEFINE_FLAGS_AND_RESULT_AND_PARAMETERS(                                    \
      CallInterfaceDescriptorData::kNoContext, StackArgumentOrder::kDefault, \
      1, ##__VA_ARGS__)

#define DEFINE_PARAMETERS_VARARGS(...)                                        \
  DEFINE_FLAGS_AND_RESULT_AND_PARAMETERS(                                     \
      CallInterfaceDescriptorData::kAllowVarArgs, StackArgumentOrder::kJS, 1, \
      ##__VA_ARGS__)

#define DEFINE_RESULT_AND_PARAMETER_TYPES_WITH_FLAG(flag, ...)                \
  void InitializePlatformIndependent(CallInterfaceDescriptorData* data)       \
      override {                                                              \
    MachineType machine_types[] = {__VA_ARGS__};                              \
    static_assert(                                                            \
        kReturnCount + kParameterCount == arraysize(machine_types),           \
        "Parameter names definition is not consistent with parameter types"); \
    data->InitializePlatformIndependent(                                      \
        Flags(flag | kDescriptorFlags), kReturnCount, kParameterCount,        \
        machine_types, arraysize(machine_types), kStackArgumentOrder);        \
  }

#define DEFINE_RESULT_AND_PARAMETER_TYPES(...) \
  DEFINE_RESULT_AND_PARAMETER_TYPES_WITH_FLAG( \
      CallInterfaceDescriptorData::kNoFlags, __VA_ARGS__)

#define DEFINE_PARAMETER_TYPES(...)                                        \
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::AnyTagged() /* result */, \
                                    ##__VA_ARGS__)

// When the extra arguments described here are located in the stack, they are
// just above the return address in the frame (first arguments).
#define DEFINE_JS_PARAMETERS(...)                           \
  static constexpr int kDescriptorFlags =                   \
      CallInterfaceDescriptorData::kAllowVarArgs;           \
  static constexpr int kReturnCount = 1;                    \
  static constexpr StackArgumentOrder kStackArgumentOrder = \
      StackArgumentOrder::kJS;                              \
  enum ParameterIndices {                                   \
    kTarget,                                                \
    kNewTarget,                                             \
    kActualArgumentsCount,                                  \
    ##__VA_ARGS__,                                          \
    kParameterCount,                                        \
    kContext = kParameterCount /* implicit parameter */     \
  };

#define DEFINE_JS_PARAMETERS_NO_CONTEXT(...)                \
  static constexpr int kDescriptorFlags =                   \
      CallInterfaceDescriptorData::kAllowVarArgs |          \
      CallInterfaceDescriptorData::kNoContext;              \
  static constexpr int kReturnCount = 1;                    \
  static constexpr StackArgumentOrder kStackArgumentOrder = \
      StackArgumentOrder::kJS;                              \
  enum ParameterIndices {                                   \
    kTarget,                                                \
    kNewTarget,                                             \
    kActualArgumentsCount,                                  \
    ##__VA_ARGS__,                                          \
    kParameterCount,                                        \
  };

#define DEFINE_JS_PARAMETER_TYPES(...)                                         \
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(), /* kTarget */               \
                         MachineType::AnyTagged(), /* kNewTarget */            \
                         MachineType::Int32(),     /* kActualArgumentsCount */ \
                         ##__VA_ARGS__)

#define DECLARE_DESCRIPTOR(name, base)                                         \
  DECLARE_DESCRIPTOR_WITH_BASE(name, base)                                     \
 protected:                                                                    \
  void InitializePlatformSpecific(CallInterfaceDescriptorData* data) override; \
  name(CallDescriptors::Key key) : base(key) {}                                \
                                                                               \
 public:

class V8_EXPORT_PRIVATE VoidDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS()
  DEFINE_PARAMETER_TYPES()
  DECLARE_DESCRIPTOR(VoidDescriptor, CallInterfaceDescriptor)
};

// This class is subclassed by Torque-generated call interface descriptors.
template <int return_count, int parameter_count, bool has_context_parameter>
class TorqueInterfaceDescriptor : public CallInterfaceDescriptor {
 public:
  static constexpr int kDescriptorFlags =
      has_context_parameter ? CallInterfaceDescriptorData::kNoFlags
                            : CallInterfaceDescriptorData::kNoContext;
  static constexpr int kParameterCount = parameter_count;
  enum ParameterIndices { kContext = kParameterCount };
  template <int i>
  static ParameterIndices ParameterIndex() {
    STATIC_ASSERT(0 <= i && i < kParameterCount);
    return static_cast<ParameterIndices>(i);
  }
  static constexpr int kReturnCount = return_count;

  using CallInterfaceDescriptor::CallInterfaceDescriptor;

 protected:
  static const int kRegisterParams =
      kParameterCount > kMaxTFSBuiltinRegisterParams
          ? kMaxTFSBuiltinRegisterParams
          : kParameterCount;
  static const int kStackParams = kParameterCount - kRegisterParams;
  virtual std::vector<MachineType> ReturnType() = 0;
  virtual std::array<MachineType, kParameterCount> ParameterTypes() = 0;
  void InitializePlatformSpecific(CallInterfaceDescriptorData* data) override {
    DefaultInitializePlatformSpecific(data, kRegisterParams);
  }
  void InitializePlatformIndependent(
      CallInterfaceDescriptorData* data) override {
    std::vector<MachineType> machine_types = ReturnType();
    DCHECK_EQ(kReturnCount, machine_types.size());
    auto parameter_types = ParameterTypes();
    machine_types.insert(machine_types.end(), parameter_types.begin(),
                         parameter_types.end());
    DCHECK_EQ(kReturnCount + kParameterCount, machine_types.size());
    data->InitializePlatformIndependent(Flags(kDescriptorFlags), kReturnCount,
                                        kParameterCount, machine_types.data(),
                                        static_cast<int>(machine_types.size()),
                                        StackArgumentOrder::kDefault);
  }
};

// Dummy descriptor used to mark builtins that don't yet have their proper
// descriptor associated.
using DummyDescriptor = VoidDescriptor;

// Dummy descriptor that marks builtins with C calling convention.
using CCallDescriptor = VoidDescriptor;

// Marks deoptimization entry builtins. Precise calling conventions currently
// differ based on the platform.
// TODO(jgruber): Once this is unified, we could create a better description
// here.
using DeoptimizationEntryDescriptor = VoidDescriptor;

class AllocateDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kRequestedSize)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::TaggedPointer(),  // result 1
                                    MachineType::IntPtr())  // kRequestedSize
  DECLARE_DESCRIPTOR(AllocateDescriptor, CallInterfaceDescriptor)
};

// This descriptor defines the JavaScript calling convention that can be used
// by stubs: target, new.target, argc (not including the receiver) and context
// are passed in registers while receiver and the rest of the JS arguments are
// passed on the stack.
class JSTrampolineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_JS_PARAMETERS()
  DEFINE_JS_PARAMETER_TYPES()

  DECLARE_JS_COMPATIBLE_DESCRIPTOR(JSTrampolineDescriptor,
                                   CallInterfaceDescriptor, 0)
};

class ContextOnlyDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS()
  DEFINE_PARAMETER_TYPES()
  DECLARE_DESCRIPTOR(ContextOnlyDescriptor, CallInterfaceDescriptor)
};

class NoContextDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT()
  DEFINE_PARAMETER_TYPES()
  DECLARE_DESCRIPTOR(NoContextDescriptor, CallInterfaceDescriptor)
};

// LoadDescriptor is used by all stubs that implement Load/KeyedLoad ICs.
class LoadDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kReceiver, kName, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kReceiver
                         MachineType::AnyTagged(),     // kName
                         MachineType::TaggedSigned())  // kSlot
  DECLARE_DESCRIPTOR(LoadDescriptor, CallInterfaceDescriptor)

  static const Register ReceiverRegister();
  static const Register NameRegister();
  static const Register SlotRegister();
};

// LoadBaselineDescriptor is a load descriptor that does not take a context as
// input.
class LoadBaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kReceiver, kName, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kReceiver
                         MachineType::AnyTagged(),     // kName
                         MachineType::TaggedSigned())  // kSlot
  DECLARE_DESCRIPTOR(LoadBaselineDescriptor, CallInterfaceDescriptor)
};

class LoadGlobalNoFeedbackDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kName, kICKind)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kName
                         MachineType::TaggedSigned())  // kICKind
  DECLARE_DESCRIPTOR(LoadGlobalNoFeedbackDescriptor, CallInterfaceDescriptor)

  static const Register NameRegister() {
    return LoadDescriptor::NameRegister();
  }

  static const Register ICKindRegister() {
    return LoadDescriptor::SlotRegister();
  }
};

class LoadNoFeedbackDescriptor : public LoadGlobalNoFeedbackDescriptor {
 public:
  DEFINE_PARAMETERS(kReceiver, kName, kICKind)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kReceiver
                         MachineType::AnyTagged(),     // kName
                         MachineType::TaggedSigned())  // kICKind
  DECLARE_DESCRIPTOR(LoadNoFeedbackDescriptor, LoadGlobalNoFeedbackDescriptor)

  static const Register ReceiverRegister() {
    return LoadDescriptor::ReceiverRegister();
  }

  static const Register NameRegister() {
    return LoadGlobalNoFeedbackDescriptor::NameRegister();
  }

  static const Register ICKindRegister() {
    return LoadGlobalNoFeedbackDescriptor::ICKindRegister();
  }
};

class LoadGlobalDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kName, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kName
                         MachineType::TaggedSigned())  // kSlot
  DECLARE_DESCRIPTOR(LoadGlobalDescriptor, CallInterfaceDescriptor)

  static const Register NameRegister() {
    return LoadDescriptor::NameRegister();
  }

  static const Register SlotRegister() {
    return LoadDescriptor::SlotRegister();
  }
};

class LoadGlobalBaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kName, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kName
                         MachineType::TaggedSigned())  // kSlot
  DECLARE_DESCRIPTOR(LoadGlobalBaselineDescriptor, CallInterfaceDescriptor)
};

class LookupBaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kName, kDepth, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kName
                         MachineType::AnyTagged(),  // kDepth
                         MachineType::AnyTagged())  // kSlot
  DECLARE_DESCRIPTOR(LookupBaselineDescriptor, CallInterfaceDescriptor)
};

class StoreDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kReceiver, kName, kValue, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kReceiver
                         MachineType::AnyTagged(),     // kName
                         MachineType::AnyTagged(),     // kValue
                         MachineType::TaggedSigned())  // kSlot
  DECLARE_DESCRIPTOR(StoreDescriptor, CallInterfaceDescriptor)

  static const Register ReceiverRegister();
  static const Register NameRegister();
  static const Register ValueRegister();
  static const Register SlotRegister();

#if V8_TARGET_ARCH_IA32
  static const bool kPassLastArgsOnStack = true;
#else
  static const bool kPassLastArgsOnStack = false;
#endif

  // Pass value and slot through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 2 : 0;
};

class StoreBaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kReceiver, kName, kValue, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kReceiver
                         MachineType::AnyTagged(),     // kName
                         MachineType::AnyTagged(),     // kValue
                         MachineType::TaggedSigned())  // kSlot
  DECLARE_DESCRIPTOR(StoreBaselineDescriptor, CallInterfaceDescriptor)

#if V8_TARGET_ARCH_IA32
  static const bool kPassLastArgsOnStack = true;
#else
  static const bool kPassLastArgsOnStack = false;
#endif

  // Pass value and slot through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 2 : 0;
};

class StoreTransitionDescriptor : public StoreDescriptor {
 public:
  DEFINE_PARAMETERS(kReceiver, kName, kMap, kValue, kSlot, kVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kReceiver
                         MachineType::AnyTagged(),     // kName
                         MachineType::AnyTagged(),     // kMap
                         MachineType::AnyTagged(),     // kValue
                         MachineType::TaggedSigned(),  // kSlot
                         MachineType::AnyTagged())     // kVector
  DECLARE_DESCRIPTOR(StoreTransitionDescriptor, StoreDescriptor)

  static const Register MapRegister();
  static const Register SlotRegister();
  static const Register VectorRegister();

  // Pass value, slot and vector through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 3 : 0;
};

class StoreWithVectorDescriptor : public StoreDescriptor {
 public:
  DEFINE_PARAMETERS(kReceiver, kName, kValue, kSlot, kVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kReceiver
                         MachineType::AnyTagged(),     // kName
                         MachineType::AnyTagged(),     // kValue
                         MachineType::TaggedSigned(),  // kSlot
                         MachineType::AnyTagged())     // kVector
  DECLARE_DESCRIPTOR(StoreWithVectorDescriptor, StoreDescriptor)

  static const Register VectorRegister();

  // Pass value, slot and vector through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 3 : 0;
};

class StoreGlobalDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kName, kValue, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kName
                         MachineType::AnyTagged(),     // kValue
                         MachineType::TaggedSigned())  // kSlot
  DECLARE_DESCRIPTOR(StoreGlobalDescriptor, CallInterfaceDescriptor)

  static const bool kPassLastArgsOnStack =
      StoreDescriptor::kPassLastArgsOnStack;
  // Pass value and slot through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 2 : 0;

  static const Register NameRegister() {
    return StoreDescriptor::NameRegister();
  }

  static const Register ValueRegister() {
    return StoreDescriptor::ValueRegister();
  }

  static const Register SlotRegister() {
    return StoreDescriptor::SlotRegister();
  }
};

class StoreGlobalBaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kName, kValue, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kName
                         MachineType::AnyTagged(),     // kValue
                         MachineType::TaggedSigned())  // kSlot
  DECLARE_DESCRIPTOR(StoreGlobalBaselineDescriptor, CallInterfaceDescriptor)

  static const bool kPassLastArgsOnStack =
      StoreDescriptor::kPassLastArgsOnStack;
  // Pass value and slot through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 2 : 0;
};

class StoreGlobalWithVectorDescriptor : public StoreGlobalDescriptor {
 public:
  DEFINE_PARAMETERS(kName, kValue, kSlot, kVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kName
                         MachineType::AnyTagged(),     // kValue
                         MachineType::TaggedSigned(),  // kSlot
                         MachineType::AnyTagged())     // kVector
  DECLARE_DESCRIPTOR(StoreGlobalWithVectorDescriptor, StoreGlobalDescriptor)

  static const Register VectorRegister() {
    return StoreWithVectorDescriptor::VectorRegister();
  }

  // Pass value, slot and vector through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 3 : 0;
};

class LoadWithVectorDescriptor : public LoadDescriptor {
 public:
  // TODO(v8:9497): Revert the Machine type for kSlot to the
  // TaggedSigned once Torque can emit better call descriptors
  DEFINE_PARAMETERS(kReceiver, kName, kSlot, kVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kReceiver
                         MachineType::AnyTagged(),  // kName
                         MachineType::AnyTagged(),  // kSlot
                         MachineType::AnyTagged())  // kVector
  DECLARE_DESCRIPTOR(LoadWithVectorDescriptor, LoadDescriptor)

  static const Register VectorRegister();

#if V8_TARGET_ARCH_IA32
  static const bool kPassLastArgsOnStack = true;
#else
  static const bool kPassLastArgsOnStack = false;
#endif

  // Pass vector through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 1 : 0;
};

// Like LoadWithVectorDescriptor, except we pass the receiver (the object which
// should be used as the receiver for accessor function calls) and the lookup
// start object separately.
class LoadWithReceiverAndVectorDescriptor : public LoadWithVectorDescriptor {
 public:
  // TODO(v8:9497): Revert the Machine type for kSlot to the
  // TaggedSigned once Torque can emit better call descriptors
  DEFINE_PARAMETERS(kReceiver, kLookupStartObject, kName, kSlot, kVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kReceiver
                         MachineType::AnyTagged(),  // kLookupStartObject
                         MachineType::AnyTagged(),  // kName
                         MachineType::AnyTagged(),  // kSlot
                         MachineType::AnyTagged())  // kVector
  DECLARE_DESCRIPTOR(LoadWithReceiverAndVectorDescriptor,
                     LoadWithVectorDescriptor)

  static const Register LookupStartObjectRegister();

#if V8_TARGET_ARCH_IA32
  static const bool kPassLastArgsOnStack = true;
#else
  static const bool kPassLastArgsOnStack = false;
#endif

  // Pass vector through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 1 : 0;
};

class LoadWithReceiverBaselineDescriptor : public LoadBaselineDescriptor {
 public:
  // TODO(v8:9497): Revert the Machine type for kSlot to the
  // TaggedSigned once Torque can emit better call descriptors
  DEFINE_PARAMETERS_NO_CONTEXT(kReceiver, kLookupStartObject, kName, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kReceiver
                         MachineType::AnyTagged(),  // kLookupStartObject
                         MachineType::AnyTagged(),  // kName
                         MachineType::AnyTagged())  // kSlot
  DECLARE_DESCRIPTOR(LoadWithReceiverBaselineDescriptor, LoadBaselineDescriptor)
};

class LoadGlobalWithVectorDescriptor : public LoadGlobalDescriptor {
 public:
  DEFINE_PARAMETERS(kName, kSlot, kVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kName
                         MachineType::TaggedSigned(),  // kSlot
                         MachineType::AnyTagged())     // kVector
  DECLARE_DESCRIPTOR(LoadGlobalWithVectorDescriptor, LoadGlobalDescriptor)

#if V8_TARGET_ARCH_IA32
  // On ia32, LoadWithVectorDescriptor passes vector on the stack and thus we
  // need to choose a new register here.
  static const Register VectorRegister() { return edx; }
#else
  static const Register VectorRegister() {
    return LoadWithVectorDescriptor::VectorRegister();
  }
#endif
};

class DynamicCheckMapsDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kMap, kSlot, kHandler)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::Int32(),          // return val
                                    MachineType::TaggedPointer(),  // kMap
                                    MachineType::IntPtr(),         // kSlot
                                    MachineType::TaggedSigned())   // kHandler

  DECLARE_DESCRIPTOR(DynamicCheckMapsDescriptor, CallInterfaceDescriptor)
};

class FastNewObjectDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kTarget, kNewTarget)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::AnyTagged())  // kNewTarget
  DECLARE_DESCRIPTOR(FastNewObjectDescriptor, CallInterfaceDescriptor)
  static const Register TargetRegister();
  static const Register NewTargetRegister();
};

class RecordWriteDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kObject, kSlot, kRememberedSet, kFPMode)
  DEFINE_PARAMETER_TYPES(MachineType::TaggedPointer(),  // kObject
                         MachineType::Pointer(),        // kSlot
                         MachineType::TaggedSigned(),   // kRememberedSet
                         MachineType::TaggedSigned())   // kFPMode

  DECLARE_DESCRIPTOR(RecordWriteDescriptor, CallInterfaceDescriptor)
};

class EphemeronKeyBarrierDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kObject, kSlotAddress, kFPMode)
  DEFINE_PARAMETER_TYPES(MachineType::TaggedPointer(),  // kObject
                         MachineType::Pointer(),        // kSlotAddress
                         MachineType::TaggedSigned())   // kFPMode

  DECLARE_DESCRIPTOR(EphemeronKeyBarrierDescriptor, CallInterfaceDescriptor)
};

class TypeConversionDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kArgument)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged())
  DECLARE_DESCRIPTOR(TypeConversionDescriptor, CallInterfaceDescriptor)

  static const Register ArgumentRegister();
};

class TypeConversionNoContextDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kArgument)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged())
  DECLARE_DESCRIPTOR(TypeConversionNoContextDescriptor, CallInterfaceDescriptor)
};

class TypeConversion_BaselineDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kArgument, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(), MachineType::UintPtr())
  DECLARE_DESCRIPTOR(TypeConversion_BaselineDescriptor, CallInterfaceDescriptor)
};

class SingleParameterOnStackDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kArgument)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged())
  DECLARE_DESCRIPTOR(SingleParameterOnStackDescriptor, CallInterfaceDescriptor)
};

class AsyncFunctionStackParameterDescriptor final
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kPromise, kResult)
  DEFINE_PARAMETER_TYPES(MachineType::TaggedPointer(), MachineType::AnyTagged())
  DECLARE_DESCRIPTOR(AsyncFunctionStackParameterDescriptor,
                     CallInterfaceDescriptor)
};

class GetIteratorStackParameterDescriptor final
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kReceiver, kCallSlot, kFeedback, kResult)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(), MachineType::AnyTagged(),
                         MachineType::AnyTagged(), MachineType::AnyTagged())
  DECLARE_DESCRIPTOR(GetIteratorStackParameterDescriptor,
                     CallInterfaceDescriptor)
};

class GetPropertyDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kObject, kKey)
  DECLARE_DEFAULT_DESCRIPTOR(GetPropertyDescriptor, CallInterfaceDescriptor)
};

class TypeofDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kObject)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged())
  DECLARE_DESCRIPTOR(TypeofDescriptor, CallInterfaceDescriptor)
};

class CallTrampolineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kFunction, kActualArgumentsCount)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kFunction
                         MachineType::Int32())      // kActualArgumentsCount
  DECLARE_DESCRIPTOR(CallTrampolineDescriptor, CallInterfaceDescriptor)
};

class CallVarargsDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kTarget, kActualArgumentsCount, kArgumentsLength,
                            kArgumentsList)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::Int32(),      // kActualArgumentsCount
                         MachineType::Int32(),      // kArgumentsLength
                         MachineType::AnyTagged())  // kArgumentsList
  DECLARE_DESCRIPTOR(CallVarargsDescriptor, CallInterfaceDescriptor)
};

class CallForwardVarargsDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kTarget, kActualArgumentsCount, kStartIndex)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::Int32(),      // kActualArgumentsCount
                         MachineType::Int32())      // kStartIndex
  DECLARE_DESCRIPTOR(CallForwardVarargsDescriptor, CallInterfaceDescriptor)
};

class CallFunctionTemplateDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kFunctionTemplateInfo, kArgumentsCount)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kFunctionTemplateInfo
                         MachineType::IntPtr())     // kArgumentsCount
  DECLARE_DESCRIPTOR(CallFunctionTemplateDescriptor, CallInterfaceDescriptor)
};

class CallWithSpreadDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kTarget, kArgumentsCount, kSpread)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::Int32(),      // kArgumentsCount
                         MachineType::AnyTagged())  // kSpread
  DECLARE_DESCRIPTOR(CallWithSpreadDescriptor, CallInterfaceDescriptor)
};

class CallWithSpread_BaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kTarget, kArgumentsCount, kSpread, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::Int32(),      // kArgumentsCount
                         MachineType::AnyTagged(),  // kSpread
                         MachineType::UintPtr())    // kSlot
  DECLARE_DESCRIPTOR(CallWithSpread_BaselineDescriptor, CallInterfaceDescriptor)
};

class CallWithSpread_WithFeedbackDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kTarget, kArgumentsCount, kSpread, kSlot,
                            kFeedbackVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::Int32(),      // kArgumentsCount
                         MachineType::AnyTagged(),  // kSpread
                         MachineType::UintPtr(),    // kSlot
                         MachineType::AnyTagged())  // kFeedbackVector
  DECLARE_DESCRIPTOR(CallWithSpread_WithFeedbackDescriptor,
                     CallInterfaceDescriptor)
};

class CallWithArrayLikeDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kTarget, kArgumentsList)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::AnyTagged())  // kArgumentsList
  DECLARE_DESCRIPTOR(CallWithArrayLikeDescriptor, CallInterfaceDescriptor)
};

class CallWithArrayLike_WithFeedbackDescriptor
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kTarget, kArgumentsList, kSlot, kFeedbackVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::AnyTagged(),  // kArgumentsList
                         MachineType::UintPtr(),    // kSlot
                         MachineType::AnyTagged())  // kFeedbackVector
  DECLARE_DESCRIPTOR(CallWithArrayLike_WithFeedbackDescriptor,
                     CallInterfaceDescriptor)
};

class ConstructVarargsDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_JS_PARAMETERS(kArgumentsLength, kArgumentsList)
  DEFINE_JS_PARAMETER_TYPES(MachineType::Int32(),      // kArgumentsLength
                            MachineType::AnyTagged())  // kArgumentsList

  DECLARE_DESCRIPTOR(ConstructVarargsDescriptor, CallInterfaceDescriptor)
};

class ConstructForwardVarargsDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_JS_PARAMETERS(kStartIndex)
  DEFINE_JS_PARAMETER_TYPES(MachineType::Int32())
  DECLARE_DESCRIPTOR(ConstructForwardVarargsDescriptor, CallInterfaceDescriptor)
};

class ConstructWithSpreadDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_JS_PARAMETERS(kSpread)
  DEFINE_JS_PARAMETER_TYPES(MachineType::AnyTagged())
  DECLARE_DESCRIPTOR(ConstructWithSpreadDescriptor, CallInterfaceDescriptor)
};

class ConstructWithSpread_BaselineDescriptor : public CallInterfaceDescriptor {
 public:
  // Note: kSlot comes before kSpread since as an untagged value it must be
  // passed in a register.
  DEFINE_JS_PARAMETERS(kSlot, kSpread)
  DEFINE_JS_PARAMETER_TYPES(MachineType::UintPtr(),    // kSlot
                            MachineType::AnyTagged())  // kSpread
  DECLARE_DESCRIPTOR(ConstructWithSpread_BaselineDescriptor,
                     CallInterfaceDescriptor)

#if V8_TARGET_ARCH_IA32
  static const bool kPassLastArgsOnStack = true;
#else
  static const bool kPassLastArgsOnStack = false;
#endif

  // Pass spread through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 1 : 0;
};

class ConstructWithSpread_WithFeedbackDescriptor
    : public CallInterfaceDescriptor {
 public:
  // Note: kSlot comes before kSpread since as an untagged value it must be
  // passed in a register.
  DEFINE_JS_PARAMETERS(kSlot, kSpread, kFeedbackVector)
  DEFINE_JS_PARAMETER_TYPES(MachineType::UintPtr(),    // kSlot
                            MachineType::AnyTagged(),  // kSpread
                            MachineType::AnyTagged())  // kFeedbackVector
  DECLARE_DESCRIPTOR(ConstructWithSpread_WithFeedbackDescriptor,
                     CallInterfaceDescriptor)
};

class ConstructWithArrayLikeDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kTarget, kNewTarget, kArgumentsList)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::AnyTagged(),  // kNewTarget
                         MachineType::AnyTagged())  // kArgumentsList
  DECLARE_DESCRIPTOR(ConstructWithArrayLikeDescriptor, CallInterfaceDescriptor)
};

class ConstructWithArrayLike_WithFeedbackDescriptor
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kTarget, kNewTarget, kArgumentsList, kSlot, kFeedbackVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kTarget
                         MachineType::AnyTagged(),  // kNewTarget
                         MachineType::AnyTagged(),  // kArgumentsList
                         MachineType::UintPtr(),    // kSlot
                         MachineType::AnyTagged())  // kFeedbackVector
  DECLARE_DESCRIPTOR(ConstructWithArrayLike_WithFeedbackDescriptor,
                     CallInterfaceDescriptor)
};

// TODO(ishell): consider merging this with ArrayConstructorDescriptor
class ConstructStubDescriptor : public CallInterfaceDescriptor {
 public:
  // TODO(jgruber): Remove the unused allocation site parameter.
  DEFINE_JS_PARAMETERS(kAllocationSite)
  DEFINE_JS_PARAMETER_TYPES(MachineType::AnyTagged())

  // TODO(ishell): Use DECLARE_JS_COMPATIBLE_DESCRIPTOR if registers match
  DECLARE_DESCRIPTOR(ConstructStubDescriptor, CallInterfaceDescriptor)
};

class AbortDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kMessageOrMessageId)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged())
  DECLARE_DESCRIPTOR(AbortDescriptor, CallInterfaceDescriptor)
};

class ArrayConstructorDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_JS_PARAMETERS(kAllocationSite)
  DEFINE_JS_PARAMETER_TYPES(MachineType::AnyTagged())

  DECLARE_JS_COMPATIBLE_DESCRIPTOR(ArrayConstructorDescriptor,
                                   CallInterfaceDescriptor, 1)
};

class ArrayNArgumentsConstructorDescriptor : public CallInterfaceDescriptor {
 public:
  // This descriptor declares only register arguments while respective number
  // of JS arguments stay on the expression stack.
  // The ArrayNArgumentsConstructor builtin does not access stack arguments
  // directly it just forwards them to the runtime function.
  DEFINE_PARAMETERS(kFunction, kAllocationSite, kActualArgumentsCount)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kFunction,
                         MachineType::AnyTagged(),  // kAllocationSite
                         MachineType::Int32())      // kActualArgumentsCount
  DECLARE_DESCRIPTOR(ArrayNArgumentsConstructorDescriptor,
                     CallInterfaceDescriptor)
};

class ArrayNoArgumentConstructorDescriptor
    : public ArrayNArgumentsConstructorDescriptor {
 public:
  // This descriptor declares same register arguments as the parent
  // ArrayNArgumentsConstructorDescriptor and it declares indices for
  // JS arguments passed on the expression stack.
  DEFINE_PARAMETERS(kFunction, kAllocationSite, kActualArgumentsCount,
                    kFunctionParameter)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kFunction
                         MachineType::AnyTagged(),  // kAllocationSite
                         MachineType::Int32(),      // kActualArgumentsCount
                         MachineType::AnyTagged())  // kFunctionParameter
  DECLARE_DESCRIPTOR(ArrayNoArgumentConstructorDescriptor,
                     ArrayNArgumentsConstructorDescriptor)
};

class ArraySingleArgumentConstructorDescriptor
    : public ArrayNArgumentsConstructorDescriptor {
 public:
  // This descriptor declares same register arguments as the parent
  // ArrayNArgumentsConstructorDescriptor and it declares indices for
  // JS arguments passed on the expression stack.
  DEFINE_PARAMETERS(kFunction, kAllocationSite, kActualArgumentsCount,
                    kArraySizeSmiParameter, kReceiverParameter)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kFunction
                         MachineType::AnyTagged(),  // kAllocationSite
                         MachineType::Int32(),      // kActualArgumentsCount
                         // JS arguments on the stack
                         MachineType::AnyTagged(),  // kArraySizeSmiParameter
                         MachineType::AnyTagged())  // kReceiverParameter
  DECLARE_DESCRIPTOR(ArraySingleArgumentConstructorDescriptor,
                     ArrayNArgumentsConstructorDescriptor)
};

class CompareDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kLeft, kRight)
  DECLARE_DESCRIPTOR(CompareDescriptor, CallInterfaceDescriptor)
};

class BinaryOpDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kLeft, kRight)
  DECLARE_DESCRIPTOR(BinaryOpDescriptor, CallInterfaceDescriptor)
};

class BinaryOp_BaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kLeft, kRight, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kLeft
                         MachineType::AnyTagged(),  // kRight
                         MachineType::UintPtr())    // kSlot
  DECLARE_DESCRIPTOR(BinaryOp_BaselineDescriptor, CallInterfaceDescriptor)
};

// This desciptor is shared among String.p.charAt/charCodeAt/codePointAt
// as they all have the same interface.
class StringAtDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kReceiver, kPosition)
  // TODO(turbofan): Return untagged value here.
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::TaggedSigned(),  // result 1
                                    MachineType::AnyTagged(),     // kReceiver
                                    MachineType::IntPtr())        // kPosition
  DECLARE_DESCRIPTOR(StringAtDescriptor, CallInterfaceDescriptor)
};

class StringAtAsStringDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kReceiver, kPosition)
  // TODO(turbofan): Return untagged value here.
  DEFINE_RESULT_AND_PARAMETER_TYPES(
      MachineType::TaggedPointer(),  // result string
      MachineType::AnyTagged(),      // kReceiver
      MachineType::IntPtr())         // kPosition
  DECLARE_DESCRIPTOR(StringAtAsStringDescriptor, CallInterfaceDescriptor)
};

class StringSubstringDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kString, kFrom, kTo)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kString
                         MachineType::IntPtr(),     // kFrom
                         MachineType::IntPtr())     // kTo

  // TODO(turbofan): Allow builtins to return untagged values.
  DECLARE_DESCRIPTOR(StringSubstringDescriptor, CallInterfaceDescriptor)
};

class CppBuiltinAdaptorDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_JS_PARAMETERS(kCFunction)
  DEFINE_JS_PARAMETER_TYPES(MachineType::Pointer())
  DECLARE_JS_COMPATIBLE_DESCRIPTOR(CppBuiltinAdaptorDescriptor,
                                   CallInterfaceDescriptor, 1)
};

class CEntry1ArgvOnStackDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kArity,          // register argument
                    kCFunction,      // register argument
                    kPadding,        // stack argument 1 (just padding)
                    kArgcSmi,        // stack argument 2
                    kTargetCopy,     // stack argument 3
                    kNewTargetCopy)  // stack argument 4
  DEFINE_PARAMETER_TYPES(MachineType::Int32(),      // kArity
                         MachineType::Pointer(),    // kCFunction
                         MachineType::AnyTagged(),  // kPadding
                         MachineType::AnyTagged(),  // kArgcSmi
                         MachineType::AnyTagged(),  // kTargetCopy
                         MachineType::AnyTagged())  // kNewTargetCopy
  DECLARE_DESCRIPTOR(CEntry1ArgvOnStackDescriptor, CallInterfaceDescriptor)
};

class ApiCallbackDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kApiFunctionAddress, kActualArgumentsCount,
                            kCallData, kHolder)
  //                           receiver is implicit stack argument 1
  //                           argv are implicit stack arguments [2, 2 + kArgc[
  DEFINE_PARAMETER_TYPES(MachineType::Pointer(),    // kApiFunctionAddress
                         MachineType::IntPtr(),     // kActualArgumentsCount
                         MachineType::AnyTagged(),  // kCallData
                         MachineType::AnyTagged())  // kHolder
  DECLARE_DESCRIPTOR(ApiCallbackDescriptor, CallInterfaceDescriptor)
};

class ApiGetterDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kReceiver, kHolder, kCallback)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kReceiver
                         MachineType::AnyTagged(),  // kHolder
                         MachineType::AnyTagged())  // kCallback
  DECLARE_DESCRIPTOR(ApiGetterDescriptor, CallInterfaceDescriptor)

  static const Register ReceiverRegister();
  static const Register HolderRegister();
  static const Register CallbackRegister();
};

// TODO(turbofan): We should probably rename this to GrowFastElementsDescriptor.
class GrowArrayElementsDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kObject, kKey)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kObject
                         MachineType::AnyTagged())  // kKey
  DECLARE_DESCRIPTOR(GrowArrayElementsDescriptor, CallInterfaceDescriptor)

  static const Register ObjectRegister();
  static const Register KeyRegister();
};

class V8_EXPORT_PRIVATE TailCallOptimizedCodeSlotDescriptor
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kOptimizedCodeEntry)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged())  // kAccumulator
  DECLARE_DESCRIPTOR(TailCallOptimizedCodeSlotDescriptor,
                     CallInterfaceDescriptor)
};

class BaselineOutOfLinePrologueDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kCalleeContext, kClosure,
                               kJavaScriptCallArgCount, kStackFrameSize,
                               kJavaScriptCallNewTarget,
                               kInterpreterBytecodeArray)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kCalleeContext
                         MachineType::AnyTagged(),  // kClosure
                         MachineType::Int32(),      // kJavaScriptCallArgCount
                         MachineType::Int32(),      // kStackFrameSize
                         MachineType::AnyTagged(),  // kJavaScriptCallNewTarget
                         MachineType::AnyTagged())  // kInterpreterBytecodeArray
  DECLARE_DESCRIPTOR(BaselineOutOfLinePrologueDescriptor,
                     CallInterfaceDescriptor)

#if V8_TARGET_ARCH_IA32
  static const bool kPassLastArgsOnStack = true;
#else
  static const bool kPassLastArgsOnStack = false;
#endif

  // Pass bytecode array through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 1 : 0;
};

class BaselineLeaveFrameDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kParamsSize, kWeight)
  DEFINE_PARAMETER_TYPES(MachineType::Int32(),  // kParamsSize
                         MachineType::Int32())  // kWeight
  DECLARE_DESCRIPTOR(BaselineLeaveFrameDescriptor, CallInterfaceDescriptor)

  static const Register ParamsSizeRegister();
  static const Register WeightRegister();
};

class V8_EXPORT_PRIVATE InterpreterDispatchDescriptor
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kAccumulator, kBytecodeOffset, kBytecodeArray,
                    kDispatchTable)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kAccumulator
                         MachineType::IntPtr(),     // kBytecodeOffset
                         MachineType::AnyTagged(),  // kBytecodeArray
                         MachineType::IntPtr())     // kDispatchTable
  DECLARE_DESCRIPTOR(InterpreterDispatchDescriptor, CallInterfaceDescriptor)
};

class InterpreterPushArgsThenCallDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kNumberOfArguments, kFirstArgument, kFunction)
  DEFINE_PARAMETER_TYPES(MachineType::Int32(),      // kNumberOfArguments
                         MachineType::Pointer(),    // kFirstArgument
                         MachineType::AnyTagged())  // kFunction
  DECLARE_DESCRIPTOR(InterpreterPushArgsThenCallDescriptor,
                     CallInterfaceDescriptor)
};

class InterpreterPushArgsThenConstructDescriptor
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kNumberOfArguments, kFirstArgument, kConstructor,
                    kNewTarget, kFeedbackElement)
  DEFINE_PARAMETER_TYPES(MachineType::Int32(),      // kNumberOfArguments
                         MachineType::Pointer(),    // kFirstArgument
                         MachineType::AnyTagged(),  // kConstructor
                         MachineType::AnyTagged(),  // kNewTarget
                         MachineType::AnyTagged())  // kFeedbackElement
  DECLARE_DESCRIPTOR(InterpreterPushArgsThenConstructDescriptor,
                     CallInterfaceDescriptor)

#if V8_TARGET_ARCH_IA32
  static const bool kPassLastArgsOnStack = true;
#else
  static const bool kPassLastArgsOnStack = false;
#endif

  // Pass constructor, new target and feedback element through the stack.
  static const int kStackArgumentsCount = kPassLastArgsOnStack ? 3 : 0;
};

class InterpreterCEntry1Descriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_RESULT_AND_PARAMETERS(1, kNumberOfArguments, kFirstArgument,
                               kFunctionEntry)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::AnyTagged(),  // result 1
                                    MachineType::Int32(),  // kNumberOfArguments
                                    MachineType::Pointer(),  // kFirstArgument
                                    MachineType::Pointer())  // kFunctionEntry
  DECLARE_DESCRIPTOR(InterpreterCEntry1Descriptor, CallInterfaceDescriptor)
};

class InterpreterCEntry2Descriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_RESULT_AND_PARAMETERS(2, kNumberOfArguments, kFirstArgument,
                               kFunctionEntry)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::AnyTagged(),  // result 1
                                    MachineType::AnyTagged(),  // result 2
                                    MachineType::Int32(),  // kNumberOfArguments
                                    MachineType::Pointer(),  // kFirstArgument
                                    MachineType::Pointer())  // kFunctionEntry
  DECLARE_DESCRIPTOR(InterpreterCEntry2Descriptor, CallInterfaceDescriptor)
};

class ForInPrepareDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_RESULT_AND_PARAMETERS(2, kEnumerator, kVectorIndex, kFeedbackVector)
  DEFINE_RESULT_AND_PARAMETER_TYPES(
      MachineType::AnyTagged(),     // result 1 (cache array)
      MachineType::AnyTagged(),     // result 2 (cache length)
      MachineType::AnyTagged(),     // kEnumerator
      MachineType::TaggedSigned(),  // kVectorIndex
      MachineType::AnyTagged())     // kFeedbackVector
  DECLARE_DESCRIPTOR(ForInPrepareDescriptor, CallInterfaceDescriptor)
};

class ResumeGeneratorDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kValue, kGenerator)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kValue
                         MachineType::AnyTagged())  // kGenerator
  DECLARE_DESCRIPTOR(ResumeGeneratorDescriptor, CallInterfaceDescriptor)
};

class FrameDropperTrampolineDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kRestartFp)
  DEFINE_PARAMETER_TYPES(MachineType::Pointer())
  DECLARE_DESCRIPTOR(FrameDropperTrampolineDescriptor, CallInterfaceDescriptor)
};

class RunMicrotasksEntryDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_ENTRY(kRootRegisterValue, kMicrotaskQueue)
  DEFINE_PARAMETER_TYPES(MachineType::Pointer(),  // kRootRegisterValue
                         MachineType::Pointer())  // kMicrotaskQueue
  DECLARE_DESCRIPTOR(RunMicrotasksEntryDescriptor, CallInterfaceDescriptor)
};

class RunMicrotasksDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kMicrotaskQueue)
  DEFINE_PARAMETER_TYPES(MachineType::Pointer())
  DECLARE_DESCRIPTOR(RunMicrotasksDescriptor, CallInterfaceDescriptor)

  static Register MicrotaskQueueRegister();
};

class WasmFloat32ToNumberDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kValue)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::AnyTagged(),  // result
                                    MachineType::Float32())    // value
  DECLARE_DESCRIPTOR(WasmFloat32ToNumberDescriptor, CallInterfaceDescriptor)
};

class WasmFloat64ToNumberDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kValue)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::AnyTagged(),  // result
                                    MachineType::Float64())    // value
  DECLARE_DESCRIPTOR(WasmFloat64ToNumberDescriptor, CallInterfaceDescriptor)
};

class V8_EXPORT_PRIVATE I64ToBigIntDescriptor final
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kArgument)
  DEFINE_PARAMETER_TYPES(MachineType::Int64())  // kArgument
  DECLARE_DESCRIPTOR(I64ToBigIntDescriptor, CallInterfaceDescriptor)
};

// 32 bits version of the I64ToBigIntDescriptor call interface descriptor
class V8_EXPORT_PRIVATE I32PairToBigIntDescriptor final
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kLow, kHigh)
  DEFINE_PARAMETER_TYPES(MachineType::Uint32(),  // kLow
                         MachineType::Uint32())  // kHigh
  DECLARE_DESCRIPTOR(I32PairToBigIntDescriptor, CallInterfaceDescriptor)
};

class V8_EXPORT_PRIVATE BigIntToI64Descriptor final
    : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kArgument)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::Int64(),      // result 1
                                    MachineType::AnyTagged())  // kArgument
  DECLARE_DESCRIPTOR(BigIntToI64Descriptor, CallInterfaceDescriptor)
};

class V8_EXPORT_PRIVATE BigIntToI32PairDescriptor final
    : public CallInterfaceDescriptor {
 public:
  DEFINE_RESULT_AND_PARAMETERS(2, kArgument)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::Uint32(),     // result 1
                                    MachineType::Uint32(),     // result 2
                                    MachineType::AnyTagged())  // kArgument
  DECLARE_DESCRIPTOR(BigIntToI32PairDescriptor, CallInterfaceDescriptor)
};

class WasmI32AtomicWait32Descriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kAddress, kExpectedValue, kTimeoutLow,
                               kTimeoutHigh)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::Uint32(),  // result 1
                                    MachineType::Uint32(),  // kAddress
                                    MachineType::Int32(),   // kExpectedValue
                                    MachineType::Uint32(),  // kTimeoutLow
                                    MachineType::Uint32())  // kTimeoutHigh
  DECLARE_DESCRIPTOR(WasmI32AtomicWait32Descriptor, CallInterfaceDescriptor)
};

class WasmI64AtomicWait32Descriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kAddress, kExpectedValueLow, kExpectedValueHigh,
                               kTimeoutLow, kTimeoutHigh)

  DEFINE_RESULT_AND_PARAMETER_TYPES_WITH_FLAG(
      CallInterfaceDescriptorData::kNoStackScan,  // allow untagged stack params
      MachineType::Uint32(),                      // result 1
      MachineType::Uint32(),                      // kAddress
      MachineType::Uint32(),                      // kExpectedValueLow
      MachineType::Uint32(),                      // kExpectedValueHigh
      MachineType::Uint32(),                      // kTimeoutLow
      MachineType::Uint32())                      // kTimeoutHigh

#if V8_TARGET_ARCH_IA32
  static constexpr bool kPassLastArgOnStack = true;
#else
  static constexpr bool kPassLastArgOnStack = false;
#endif

  // Pass the last parameter through the stack.
  static constexpr int kStackArgumentsCount = kPassLastArgOnStack ? 1 : 0;

  DECLARE_DESCRIPTOR(WasmI64AtomicWait32Descriptor, CallInterfaceDescriptor)
};

class CloneObjectWithVectorDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kSource, kFlags, kSlot, kVector)
  DEFINE_RESULT_AND_PARAMETER_TYPES(MachineType::TaggedPointer(),  // result 1
                                    MachineType::AnyTagged(),      // kSource
                                    MachineType::TaggedSigned(),   // kFlags
                                    MachineType::TaggedSigned(),   // kSlot
                                    MachineType::AnyTagged())      // kVector
  DECLARE_DESCRIPTOR(CloneObjectWithVectorDescriptor, CallInterfaceDescriptor)
};

class CloneObjectBaselineDescriptor final : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kSource, kFlags, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),     // kSource
                         MachineType::TaggedSigned(),  // kFlags
                         MachineType::TaggedSigned())  // kSlot
  DECLARE_DESCRIPTOR(CloneObjectBaselineDescriptor, CallInterfaceDescriptor)
};

class BinaryOp_WithFeedbackDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kLeft, kRight, kSlot, kFeedbackVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kLeft
                         MachineType::AnyTagged(),  // kRight
                         MachineType::UintPtr(),    // kSlot
                         MachineType::AnyTagged())  // kFeedbackVector
  DECLARE_DESCRIPTOR(BinaryOp_WithFeedbackDescriptor, CallInterfaceDescriptor)
};

class CallTrampoline_BaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kFunction, kActualArgumentsCount, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kFunction
                         MachineType::Int32(),      // kActualArgumentsCount
                         MachineType::UintPtr())    // kSlot
  DECLARE_DESCRIPTOR(CallTrampoline_BaselineDescriptor, CallInterfaceDescriptor)
};

class CallTrampoline_WithFeedbackDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_VARARGS(kFunction, kActualArgumentsCount, kSlot,
                            kFeedbackVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kFunction
                         MachineType::Int32(),      // kActualArgumentsCount
                         MachineType::UintPtr(),    // kSlot
                         MachineType::AnyTagged())  // kFeedbackVector
  DECLARE_DESCRIPTOR(CallTrampoline_WithFeedbackDescriptor,
                     CallInterfaceDescriptor)
};

class Compare_WithFeedbackDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kLeft, kRight, kSlot, kFeedbackVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kLeft
                         MachineType::AnyTagged(),  // kRight
                         MachineType::UintPtr(),    // kSlot
                         MachineType::AnyTagged())  // kFeedbackVector
  DECLARE_DESCRIPTOR(Compare_WithFeedbackDescriptor, CallInterfaceDescriptor)
};

class Compare_BaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS_NO_CONTEXT(kLeft, kRight, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kLeft
                         MachineType::AnyTagged(),  // kRight
                         MachineType::UintPtr())    // kSlot
  DECLARE_DESCRIPTOR(Compare_BaselineDescriptor, CallInterfaceDescriptor)
};

class Construct_BaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_JS_PARAMETERS_NO_CONTEXT(kSlot)
  DEFINE_JS_PARAMETER_TYPES(MachineType::UintPtr())  // kSlot
  DECLARE_JS_COMPATIBLE_DESCRIPTOR(Construct_BaselineDescriptor,
                                   CallInterfaceDescriptor, 1)
};

class Construct_WithFeedbackDescriptor : public CallInterfaceDescriptor {
 public:
  // kSlot is passed in a register, kFeedbackVector on the stack.
  DEFINE_JS_PARAMETERS(kSlot, kFeedbackVector)
  DEFINE_JS_PARAMETER_TYPES(MachineType::UintPtr(),    // kSlot
                            MachineType::AnyTagged())  // kFeedbackVector
  DECLARE_JS_COMPATIBLE_DESCRIPTOR(Construct_WithFeedbackDescriptor,
                                   CallInterfaceDescriptor, 1)
};

class UnaryOp_WithFeedbackDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kValue, kSlot, kFeedbackVector)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kValue
                         MachineType::UintPtr(),    // kSlot
                         MachineType::AnyTagged())  // kFeedbackVector
  DECLARE_DESCRIPTOR(UnaryOp_WithFeedbackDescriptor, CallInterfaceDescriptor)
};

class UnaryOp_BaselineDescriptor : public CallInterfaceDescriptor {
 public:
  DEFINE_PARAMETERS(kValue, kSlot)
  DEFINE_PARAMETER_TYPES(MachineType::AnyTagged(),  // kValue
                         MachineType::UintPtr())    // kSlot
  DECLARE_DESCRIPTOR(UnaryOp_BaselineDescriptor, CallInterfaceDescriptor)
};

#define DEFINE_TFS_BUILTIN_DESCRIPTOR(Name, ...)                          \
  class Name##Descriptor : public CallInterfaceDescriptor {               \
   public:                                                                \
    DEFINE_PARAMETERS(__VA_ARGS__)                                        \
    DECLARE_DEFAULT_DESCRIPTOR(Name##Descriptor, CallInterfaceDescriptor) \
  };
BUILTIN_LIST_TFS(DEFINE_TFS_BUILTIN_DESCRIPTOR)
#undef DEFINE_TFS_BUILTIN_DESCRIPTOR

// This file contains interface descriptor class definitions for builtins
// defined in Torque. It is included here because the class definitions need to
// precede the definition of name##Descriptor::key() below.
#include "torque-generated/interface-descriptors.inc"

#undef DECLARE_DEFAULT_DESCRIPTOR
#undef DECLARE_DESCRIPTOR_WITH_BASE
#undef DECLARE_DESCRIPTOR
#undef DECLARE_JS_COMPATIBLE_DESCRIPTOR
#undef DEFINE_FLAGS_AND_RESULT_AND_PARAMETERS
#undef DEFINE_RESULT_AND_PARAMETERS
#undef DEFINE_PARAMETERS
#undef DEFINE_PARAMETERS_VARARGS
#undef DEFINE_PARAMETERS_NO_CONTEXT
#undef DEFINE_RESULT_AND_PARAMETER_TYPES
#undef DEFINE_PARAMETER_TYPES
#undef DEFINE_JS_PARAMETERS
#undef DEFINE_JS_PARAMETER_TYPES

// We define the association between CallDescriptors::Key and the specialized
// descriptor here to reduce boilerplate and mistakes.
#define DEF_KEY(name, ...) \
  CallDescriptors::Key name##Descriptor::key() { return CallDescriptors::name; }
INTERFACE_DESCRIPTOR_LIST(DEF_KEY)
#undef DEF_KEY
}  // namespace internal
}  // namespace v8

#endif  // V8_CODEGEN_INTERFACE_DESCRIPTORS_H_
