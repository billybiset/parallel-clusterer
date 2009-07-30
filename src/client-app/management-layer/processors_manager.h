#ifndef PROCESSORS_MANAGER_H
#define PROCESSORS_MANAGER_H

#include <string>

#include "client_processor.h"
#include "job_unit.h"

namespace parallel_clusterer
{
    class ProcessorsManager
    {
        public:
            static ProcessorsManager* get_instance();

//             bool   deliver_message(const std::string& message);
            void   deliver_message(const std::string& message);

            void   register_processor(ClientProcessor* client_processor);

            void   wrap_process(const std::string& message);

            const std::string& get_return_message() const;

            virtual ~ProcessorsManager() {}
        protected:
            
        private:
            /*methods*/
            ProcessorsManager();
            /*attributes*/
            static ProcessorsManager* _instance;
            ClientProcessor*          _processor;
    };
}

#endif
