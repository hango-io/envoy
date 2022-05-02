1.21.5 (July 25, 2022)
======================

Incompatible Behavior Changes
-----------------------------
*Changes that are expected to cause an incompatibility if applicable; deployment changes are likely required*

Minor Behavior Changes
----------------------
*Changes that may cause incompatibilities for some users, but should not for most*

* access_log: added new access_log command operator ``%ENVIRONMENT(X):Z%``.
* access_log: log all header values in the grpc access log.
* config: warning messages for protobuf unknown fields now contain ancestors for easier troubleshooting.
* decompressor: decompressor does not duplicate `accept-encoding` header values anymore. This behavioral change can be reverted by setting runtime guard ``envoy.reloadable_features.append_to_accept_content_encoding_only_once`` to false.
* dynamic_forward_proxy: if a DNS resolution fails, failing immediately with a specific resolution error, rather than finishing up all local filters and failing to select an upstream host.
* ext_authz: added requested server name in ext_authz network filter for auth review.
* file: changed disk based files to truncate files which are not being appended to. This behavioral change can be temporarily reverted by setting runtime guard ``envoy.reloadable_features.append_or_truncate`` to false.
* grpc: flip runtime guard ``envoy.reloadable_features.enable_grpc_async_client_cache`` to be default enabled. async grpc client created through getOrCreateRawAsyncClient will be cached by default.
* http: now the max concurrent streams of http2 connection can not only be adjusted down according to the SETTINGS frame but also can be adjusted up, of course, it can not exceed the configured upper bounds. This fix is guarded by ``envoy.reloadable_features.http2_allow_capacity_increase_by_settings``.
* http: the behavior of the :ref:`timeout <envoy_v3_api_field_config.core.v3.KeepaliveSettings.timeout>`
  field has been modified to extend the timeout when *any* frame is received on the owning HTTP/2
  connection. This negates the effect of head-of-line (HOL) blocking for slow connections. If
  any frame is received the assumption is that the connection is working. This behavior change
  can be reverted by setting the ``envoy.reloadable_features.http2_delay_keepalive_timeout`` runtime
  flag to false.
* lua: export symbols of LuaJit by default on Linux. This is useful in cases where you have a lua script
  that loads shared object libraries, such as those installed via luarocks.
* thrift: add validate_clusters in :ref:`RouteConfiguration <envoy_v3_api_msg_extensions.filters.network.thrift_proxy.v3.RouteConfiguration>` to override the default behavior of cluster validation.
* tls: if both :ref:`match_subject_alt_names <envoy_v3_api_field_extensions.transport_sockets.tls.v3.CertificateValidationContext.match_subject_alt_names>` and :ref:`match_typed_subject_alt_names <envoy_v3_api_field_extensions.transport_sockets.tls.v3.CertificateValidationContext.match_typed_subject_alt_names>` are specified, the former (deprecated) field is ignored. Previously, setting both fields would result in an error.

Bug Fixes
---------
*Changes expected to improve the state of the world and are unlikely to have negative effects*

* docker: update Docker images (``distroless`` -> ``49d2923f35d6``) to resolve CVE issues in container packages.

Removed Config or Runtime
-------------------------
*Normally occurs at the end of the* :ref:`deprecation period <deprecated>`

New Features
------------


Deprecated
----------
