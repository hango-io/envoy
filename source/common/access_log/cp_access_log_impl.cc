#include "source/common/access_log/cp_access_log_impl.h"
#include "envoy/config/accesslog/v3/accesslog.pb.h"
#include "envoy/config/accesslog/v3/accesslog.pb.validate.h"

#include "source/common/common/utility.h"
#include "source/common/config/utility.h"
#include "source/common/protobuf/utility.h"

namespace Envoy {
namespace AccessLog {

CrossProtoInstanceSharedPtr
CrossProtoAccessLogFactory::fromProto(const envoy::config::accesslog::v3::AccessLog& config,
                                      Server::Configuration::FactoryContext& context,
                                      const std::string& formatter_namespace) {
  ExtensionFilterPtr filter;
  if (config.has_filter()) {
    filter = ExtensionAccessLogFilterFactory::fromProto(config.filter(), context.runtime(),
                                                        context.api().randomGenerator(),
                                                        context.messageValidationVisitor());
  }
  auto& formatter_factory =
      Config::Utility::getAndCheckFactoryByName<CrossProtoLogFormatterFactory>(formatter_namespace);

  auto& factory =
      Config::Utility::getAndCheckFactoryByName<CrossProtoAccessLogInstanceFactory>(config.name());

  ProtobufTypes::MessagePtr message = Config::Utility::translateToFactoryConfig(
      config, context.messageValidationVisitor(), factory);

  return factory.createAccessLogInstance(*message, std::move(filter), context, formatter_factory);
}

ExtensionFilterPtr ExtensionAccessLogFilterFactory::fromProto(
    const envoy::config::accesslog::v3::AccessLogFilter& config, Runtime::Loader& runtime,
    Random::RandomGenerator& random, ProtobufMessage::ValidationVisitor& validation_visitor) {
  switch (config.filter_specifier_case()) {
  case envoy::config::accesslog::v3::AccessLogFilter::FilterSpecifierCase::kExtensionFilter:
    MessageUtil::validate(config, validation_visitor);
    {
      auto& factory = Config::Utility::getAndCheckFactory<ExtensionAccessLogExtensionFilterFactory>(
          config.extension_filter());
      return factory.createFilter(config.extension_filter(), runtime, random);
    }
  default:
    PANIC_DUE_TO_CORRUPT_ENUM;
  }
  PANIC_DUE_TO_CORRUPT_ENUM;
}

} // namespace AccessLog
} // namespace Envoy
