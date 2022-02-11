#include <memory>
#include <unordered_map>

#include "envoy/extensions/access_loggers/file/v3/file.pb.validate.h"
#include "envoy/registry/registry.h"
#include "envoy/server/filter_config.h"

// #include "source/common/access_log/access_log_formatter.h"
#include "source/common/access_log/cp_access_log_impl.h"
#include "source/common/common/logger.h"
#include "source/common/protobuf/protobuf.h"

namespace Envoy {
namespace Extensions {
namespace AccessLoggers {
namespace File {

class CrossProtoFileAccessLog : public AccessLog::CrossProtoInstance {
public:
  CrossProtoFileAccessLog(const std::string& access_log_path,
                          AccessLog::ExtensionFilterPtr&& filter,
                          AccessLog::CrossProtoFormatterPtr&& formatter,
                          AccessLog::AccessLogManager& log_manager);

private:
  void log(const void* context) override;

  AccessLog::AccessLogFileSharedPtr log_file_;
  AccessLog::CrossProtoFormatterPtr formatter_;
  AccessLog::ExtensionFilterPtr filter_;
};

/**
 * Config registration for the file access log. @see AccessLogInstanceFactory.
 */
class CrossProtoFileAccessLogFactory : public AccessLog::CrossProtoAccessLogInstanceFactory {
public:
  AccessLog::CrossProtoInstanceSharedPtr
  createAccessLogInstance(const Protobuf::Message& config, AccessLog::ExtensionFilterPtr&& filter,
                          Server::Configuration::FactoryContext& context,
                          AccessLog::CrossProtoLogFormatterFactory& formatter_factory) override;

  ProtobufTypes::MessagePtr createEmptyConfigProto() override {
    return ProtobufTypes::MessagePtr{
        new envoy::extensions::access_loggers::file::v3::FileAccessLog()};
  }

  std::string name() const override { return "envoy.file_access_log.extensions"; }
  std::string category() const override { return "envoy.file_access_log"; }

private:
  std::unordered_map<std::string, std::string> convertJsonFormatToMap(ProtobufWkt::Struct config);
};

} // namespace File
} // namespace AccessLoggers
} // namespace Extensions
} // namespace Envoy
