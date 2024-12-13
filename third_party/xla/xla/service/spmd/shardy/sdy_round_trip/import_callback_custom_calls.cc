/* Copyright 2024 The OpenXLA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "xla/service/spmd/shardy/sdy_round_trip/import_callback_custom_calls.h"

#include <memory>

#include "llvm/ADT/StringRef.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassRegistry.h"
#include "mlir/Support/LLVM.h"
#include "mlir/Support/TypeID.h"
#include "stablehlo/dialect/StablehloOps.h"
#include "xla/service/spmd/shardy/utils.h"

namespace xla {
namespace sdy {

namespace {

using ::mlir::ModuleOp;
using ::mlir::StringRef;
using ::mlir::stablehlo::CustomCallOp;

class SdyRoundTripImportCallbackCustomCallsPass
    : public mlir::PassWrapper<SdyRoundTripImportCallbackCustomCallsPass,
                               mlir::OperationPass<ModuleOp>> {
 public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(
      SdyRoundTripImportCallbackCustomCallsPass)

  void runOnOperation() final {
    getOperation().walk([&](CustomCallOp op) {
      StringRef targetName = op.getCallTargetName();
      if (op->getNumResults() != 0 ||
          (targetName != "xla_python_cpu_callback" &&
           targetName != "xla_python_gpu_callback")) {
        return;
      }
      mlir::IRRewriter rewriter(op);
      CustomCallOp newCustomCall =
          buildCustomCall(op, op->getOperand(0).getType(), rewriter);
      newCustomCall.setResultLayoutsAttr(rewriter.getArrayAttr(
          {op.getOperandLayoutsAttr().getValue().front()}));
      rewriter.eraseOp(op);
    });
  }

  StringRef getArgument() const override {
    return "xla-sdy-round-trip-import-callback-custom-calls";
  }

  StringRef getDescription() const override {
    return "Modifies the return types of XLA host callback custom calls to be "
           "compatible with SDY";
  }

  void getDependentDialects(mlir::DialectRegistry& registry) const final {
    registry.insert<mlir::stablehlo::StablehloDialect>();
  }
};

}  // namespace

std::unique_ptr<mlir::Pass> createSdyRoundTripImportCallbackCustomCallsPass() {
  return std::make_unique<SdyRoundTripImportCallbackCustomCallsPass>();
}

void registerSdyRoundTripImportCallbackCustomCallsPass() {
  mlir::registerPass(createSdyRoundTripImportCallbackCustomCallsPass);
}

}  // namespace sdy
}  // namespace xla
