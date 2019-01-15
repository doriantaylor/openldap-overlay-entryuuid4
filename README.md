# Overlay `entryuuid4`

This overlay supplants the default behaviour for generating `entryUUID`
attributes. By default, slapd generates Version 1 (DCE, time and
MAC address-based) UUIDs. This small piece of code simply swaps in
Version 4 (random, [RFC4122](https://tools.ietf.org/html/rfc4122))
UUIDs instead.
