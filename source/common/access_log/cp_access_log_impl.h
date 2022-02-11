#pragma once

#include <string>

#include "access_log_impl.h"
#include "source/common/access_log/access_log_impl.h"

namespace Envoy {
namespace AccessLog {

/**
 * Interface for access log filters
 */
class ExtensionFilter {
public:
  virtual ~ExtensionFilter() = default;

  /**
   * Evaluate whether an access log should be written based on request and response data.
   * @return TRUE if the log should be written.
   */
  virtual bool evaluate(const void* context) const PURE;
};

using ExtensionFilterPtr = std::unique_ptr<ExtensionFilter>;

/**
 * Access log filter factory that reads from proto.
 */
class ExtensionAccessLogFilterFactory {
public:
  /**
   * Read a filter definition from proto and instantiate a concrete filter class.
   */

  static ExtensionFilterPtr fromProto(const envoy::config::accesslog::v3::AccessLogFilter& config,
                                      Runtime::Loader& runtime, Random::RandomGenerator& random,
                                      ProtobufMessage::ValidationVisitor& validation_visitor);
};

/**
 * Extension filter factory that reads from ExtensionFilter proto for ExtensionAccessLog.
 */
class ExtensionAccessLogExtensionFilterFactory : public Config::TypedFactory {
public:
  ~ExtensionAccessLogExtensionFilterFactory() override = default;

  /**
   * Create a particular extension filter implementation from a config proto. If the
   * implementation is unable to produce a filter with the provided parameters, it should throw an
   * EnvoyException. The returned pointer should never be nullptr.
   * @param config supplies the custom configuration for this filter type.
   * @param runtime supplies the runtime loader.
   * @param random supplies the random generator.
   * @return an instance of extension filter implementation from a config proto.
   */
  virtual ExtensionFilterPtr createFilter(const envoy::config::accesslog::v3::ExtensionFilter& config,
                                 Runtime::Loader& runtime, Random::RandomGenerator& random) PURE;

  std::string category() const override { return "envoy.extension_access_logger.extension_filters"; }
};

class CrossProtoFormatter {
public:
  virtual ~CrossProtoFormatter() = default;

  virtual std::string format(const void* context) const PURE;
};

using CrossProtoFormatterPtr = std::unique_ptr<CrossProtoFormatter>;

class CrossProtoInstance {
public:
  virtual ~CrossProtoInstance() = default;

  virtual void log(const void* context) PURE;
};

using CrossProtoInstanceSharedPtr = std::shared_ptr<CrossProtoInstance>;

// Implement by every network filter that need use access log like dubbo proxy, thrift proxy, and
// http connection manager etc.
class CrossProtoLogFormatterFactory : public Config::UntypedFactory {
public:
  virtual AccessLog::FilterPtr createFilter(const envoy::config::accesslog::v3::AccessLogFilter&,
                                            Runtime::Loader&, Random::RandomGenerator&,
                                            ProtobufMessage::ValidationVisitor&) {
    return nullptr;
  }

  virtual AccessLog::CrossProtoFormatterPtr createFormatter(const std::string& text_format) PURE;
  virtual AccessLog::CrossProtoFormatterPtr
  createFormatter(const std::unordered_map<std::string, std::string>& json_format) PURE;
};

// Implement by every sink like file, grpc, kafka etc.
class CrossProtoAccessLogInstanceFactory : public Config::TypedFactory {
public:
  virtual AccessLog::CrossProtoInstanceSharedPtr
  createAccessLogInstance(const Protobuf::Message& config, AccessLog::ExtensionFilterPtr&& filter,
                          Server::Configuration::FactoryContext& context,
                          CrossProtoLogFormatterFactory& formatter_factory) PURE;
};

/**
 * Access log factory that reads the configuration from proto.
 */
class CrossProtoAccessLogFactory {
public:
  static CrossProtoInstanceSharedPtr
  fromProto(const envoy::config::accesslog::v3::AccessLog& config,
            Server::Configuration::FactoryContext& context, const std::string& formatter_namespace);
};

} // namespace AccessLog
} // namespace Envoy
