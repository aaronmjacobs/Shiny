#ifndef SHINY_DELEGATE_H
#define SHINY_DELEGATE_H

#include "Shiny/Pointers.h"
#include "Shiny/ShinyAssert.h"

#include <algorithm>
#include <functional>
#include <vector>

namespace Shiny {

template<typename RetType, typename... Params>
class Delegate {
public:
   using ReturnType = RetType;
   using FuncType = std::function<ReturnType(Params...)>;

   class HandleData {
   public:
      HandleData(const FuncType& inFunc = {})
         : func(inFunc) {
      }
      HandleData(const HandleData& other) = delete;
      HandleData(HandleData&& other) = delete;
      ~HandleData() = default;
      HandleData& operator=(const HandleData& other) = delete;
      HandleData& operator=(HandleData&& other) = delete;

   private:
      friend class Delegate;
      FuncType func;
   };

   using Handle = SPtr<HandleData>;
   using Binding = WPtr<HandleData>;

   Handle bind(const FuncType& function) {
      Handle handle = std::make_shared<HandleData>(function);
      bindings.push_back(Binding(handle));
      return handle;
   }

   void execute(Params... params) {
      purgeUnbound();

      for (const Binding& binding : bindings) {
         Handle handle = binding.lock();
         ASSERT(handle);
         handle->func(params...);
      }
   }

   std::vector<ReturnType> executeWithReturn(Params... params) {
      purgeUnbound();

      std::vector<ReturnType> returnValues(bindings.size());

      std::size_t index = 0;
      for (const Binding& binding : bindings) {
         Handle handle = binding.lock();
         ASSERT(handle);
         returnValues[index++] = handle->func(params...);
      }

      return returnValues;
   }

private:
   void purgeUnbound() {
      bindings.erase(std::remove_if(bindings.begin(), bindings.end(), [](const Binding& binding) { return binding.expired(); }), bindings.end());
   }

   std::vector<Binding> bindings;
};

} // namespace Shiny

#endif
