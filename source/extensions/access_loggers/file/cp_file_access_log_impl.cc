#include "source/extensions/access_loggers/file/cp_file_access_log_impl.h"

namespace Envoy {
namespace Extensions {
namespace AccessLoggers {
namespace File {

CrossProtoFileAccessLog::CrossProtoFileAccessLog(const std::string& access_log_path,
                                                 AccessLog::ExtensionFilterPtr&& filter,
                                                 AccessLog::CrossProtoFormatterPtr&& formatter,
                                                 AccessLog::AccessLogManager& log_manager)
    : formatter_(std::move(formatter)), filter_(std::move(filter)) {
  log_file_ = log_manager.createAccessLog({Filesystem::DestinationType::File, access_log_path});
}

void CrossProtoFileAccessLog::log(const void* context) {
  if (filter_ && !filter_->evaluate(context)) {
    return;
  }
  log_file_->write(formatter_->format(context));
}

AccessLog::CrossProtoInstanceSharedPtr CrossProtoFileAccessLogFactory::createAccessLogInstance(
    const Protobuf::Message& config, AccessLog::ExtensionFilterPtr&& filter,
    Server::Configuration::FactoryContext& context,
    AccessLog::CrossProtoLogFormatterFactory& formatter_factory) {
  const auto& fal_config = MessageUtil::downcastAndValidate<
      const envoy::extensions::access_loggers::file::v3::FileAccessLog&>(
      config, context.messageValidationVisitor());
  AccessLog::CrossProtoFormatterPtr formatter;

  if (fal_config.access_log_format_case() ==
          envoy::extensions::access_loggers::file::v3::FileAccessLog::kFormat ||
      fal_config.access_log_format_case() ==
          envoy::extensions::access_loggers::file::v3::FileAccessLog::ACCESS_LOG_FORMAT_NOT_SET) {
    formatter = formatter_factory.createFormatter(fal_config.format());
  } else if (fal_config.access_log_format_case() ==
             envoy::extensions::access_loggers::file::v3::FileAccessLog::kJsonFormat) {
    auto json_format_map = this->convertJsonFormatToMap(fal_config.json_format());
    formatter = formatter_factory.createFormatter(json_format_map);
  } else {
    throw EnvoyException(
        "Invalid access_log format provided. Only 'format' and 'json_format' are supported.");
  }

  return std::make_shared<CrossProtoFileAccessLog>(
      fal_config.path(), std::move(filter), std::move(formatter), context.accessLogManager());
}

std::unordered_map<std::string, std::string>
CrossProtoFileAccessLogFactory::convertJsonFormatToMap(ProtobufWkt::Struct json_format) {
  std::unordered_map<std::string, std::string> output;
  for (const auto& pair : json_format.fields()) {
    if (pair.second.kind_case() != ProtobufWkt::Value::kStringValue) {
      throw EnvoyException("Only string values are supported in the JSON access log format.");
    }
    output.emplace(pair.first, pair.second.string_value());
  }
  return output;
}

REGISTER_FACTORY(CrossProtoFileAccessLogFactory, AccessLog::CrossProtoAccessLogInstanceFactory);

} // namespace File
} // namespace AccessLoggers
} // namespace Extensions
} // namespace Envoy
