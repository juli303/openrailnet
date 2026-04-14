#ifndef ORN_PROTO_H
#define ORN_PROTO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __GNUC__
#define ORN_PACKED __attribute__((packed))
#else
#define ORN_PACKED
#pragma pack(push, 1)
#endif

/* ============================================================
 * OpenRailNet Protocol - Draft v0.1
 * Frame format:
 *   [PRE1][PRE2][HEADER][PAYLOAD][CRC16]
 * CRC16 covers HEADER + PAYLOAD, not the preamble.
 * ============================================================ */

/* ============================================================
 * Constants
 * ============================================================ */

#define ORN_PREAMBLE_1              0x55u
#define ORN_PREAMBLE_2              0xD5u
#define ORN_PROTO_VERSION_V0_1      0x1u

#define ORN_NODE_ID_BROADCAST       0xFFFFu
#define ORN_MAX_PAYLOAD_LEN         255u

/* ============================================================
 * Version + Flags
 * VERFLG: upper nibble = version, lower nibble = flags
 *
 * ACK semantics:
 * - ORN_FLAG_ACK_REQUESTED asks for a reliability response to this frame.
 * - ORN_FLAG_IS_ACK marks ORN_PKT_ACK and ORN_PKT_NACK frames.
 * - ACK/NACK packets reuse orn_payload_ack_t; ACK uses code=0, NACK uses
 *   a nonzero code.
 * ============================================================ */

#define ORN_VERFLG_MAKE(version, flags) \
    (uint8_t)((((version) & 0x0Fu) << 4) | ((flags) & 0x0Fu))

#define ORN_VERFLG_VERSION(vf) \
    (uint8_t)(((vf) >> 4) & 0x0Fu)

#define ORN_VERFLG_FLAGS(vf) \
    (uint8_t)((vf) & 0x0Fu)

typedef enum
{
    ORN_FLAG_ACK_REQUESTED = (1u << 0),
    ORN_FLAG_IS_ACK        = (1u << 1),
    ORN_FLAG_FORWARDED     = (1u << 2),
    ORN_FLAG_RESERVED3     = (1u << 3)
} orn_flag_t;

/* ============================================================
 * Priority
 * ============================================================ */

typedef enum
{
    ORN_PRIO_EMERGENCY = 0,
    ORN_PRIO_CONTROL   = 1,
    ORN_PRIO_NORMAL    = 2,
    ORN_PRIO_DIAG      = 3
} orn_priority_t;

/* ============================================================
 * Packet Types
 * ============================================================ */

typedef enum
{
    ORN_PKT_HELLO            = 0x01,
    ORN_PKT_HELLO_ACK        = 0x02,

    ORN_PKT_DISCOVER_TRIGGER = 0x03,
    ORN_PKT_DISCOVER_AUTH    = 0x04,
    ORN_PKT_CONSIST_TABLE    = 0x05,

    ORN_PKT_TRAIN_COMMAND    = 0x10,
    ORN_PKT_EMERGENCY_STOP   = 0x11,

    ORN_PKT_VEHICLE_STATUS   = 0x20,
    ORN_PKT_LOCO_STATUS      = 0x21,

    ORN_PKT_ACK              = 0x30,
    ORN_PKT_NACK             = 0x31
} orn_packet_type_t;

/* ============================================================
 * Node / Vehicle Types
 * ============================================================ */

typedef enum
{
    ORN_VEHICLE_UNKNOWN = 0,
    ORN_VEHICLE_LOCO    = 1,
    ORN_VEHICLE_WAGON   = 2,
    ORN_VEHICLE_CABCAR  = 3,
    ORN_VEHICLE_SERVICE = 4
} orn_vehicle_type_t;

typedef enum
{
    ORN_FRONT_A = 0,
    ORN_FRONT_B = 1
} orn_front_side_t;

typedef enum
{
    ORN_ORIENT_NORMAL   = 0,
    ORN_ORIENT_REVERSED = 1
} orn_orientation_t;

typedef enum
{
    ORN_DIR_HOLD    = 0,
    ORN_DIR_FORWARD = 1,
    ORN_DIR_REVERSE = 2
} orn_direction_t;

/* Optional logical port marker for trigger direction */
typedef enum
{
    ORN_PORT_A = 0,
    ORN_PORT_B = 1
} orn_port_t;

/* ============================================================
 * Common Packet Header
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint8_t  verflg;    /* version + flags */
    uint8_t  type;      /* orn_packet_type_t */
    uint8_t  priority;  /* orn_priority_t */
    uint16_t src;       /* source node ID; same logical ID used as vehicle_id */
    uint16_t dst;       /* destination node ID or ORN_NODE_ID_BROADCAST */
    uint8_t  seq;       /* sender sequence number */
    uint8_t  len;       /* payload length in bytes */
} orn_header_t;

/* On-wire frame preamble, before header */
typedef struct ORN_PACKED
{
    uint8_t pre1; /* 0x55 */
    uint8_t pre2; /* 0xD5 */
} orn_preamble_t;

/* Convenient container for RAM assembly/parsing */
typedef struct ORN_PACKED
{
    orn_preamble_t preamble;
    orn_header_t   header;
    uint8_t        payload[ORN_MAX_PAYLOAD_LEN];
    /* uint16_t crc16 follows logically at payload[header.len] */
} orn_frame_t;

/* ============================================================
 * Common Capability / Status bitfields
 * ============================================================ */

typedef uint16_t orn_capability_flags_t;
typedef uint16_t orn_error_flags_t;

/* ============================================================
 * HELLO
 *
 * Used for neighbor detection and liveness.
 * The sender identity is carried in header.src. HELLO payload carries
 * self-description plus a HELLO-local sequence number for link tracking.
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint8_t  vehicle_type;     /* orn_vehicle_type_t */
    uint8_t  front_side;       /* orn_front_side_t */
    uint8_t  proto_version;    /* usually ORN_PROTO_VERSION_V0_1 */
    uint8_t  hello_seq;        /* local hello sequence */
    uint16_t capabilities;     /* orn_capability_flags_t */
} orn_payload_hello_t;

/* ============================================================
 * HELLO_ACK
 *
 * Link-management response to HELLO. This is distinct from ORN_PKT_ACK,
 * which is the generic reliability acknowledgment used with ACK_REQUESTED.
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint8_t acked_hello_seq;
    uint8_t status_flags;
} orn_payload_hello_ack_t;

/* ============================================================
 * DISCOVER_TRIGGER
 *
 * Sent in one direction only to find an end node.
 * The end node that receives it starts DISCOVER_AUTH.
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint16_t session_id;
    uint8_t  trigger_direction; /* orn_port_t from original sender perspective */
    uint8_t  reserved;
} orn_payload_discover_trigger_t;

/* ============================================================
 * DISCOVER_AUTH
 *
 * Base payload followed by record_count discovery records.
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint16_t session_id;
    uint8_t  record_count;
    uint8_t  reserved;
} orn_payload_discover_auth_t;

typedef struct ORN_PACKED
{
    uint16_t vehicle_id;
    uint8_t  vehicle_type;   /* orn_vehicle_type_t */
    uint8_t  front_side;     /* orn_front_side_t */
    uint16_t capabilities;   /* orn_capability_flags_t */
} orn_discover_record_t;

/* ============================================================
 * CONSIST_TABLE
 *
 * Base payload followed by entry_count consist entries.
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint16_t session_id;
    uint8_t  entry_count;
    uint8_t  reserved;
} orn_payload_consist_table_t;

typedef struct ORN_PACKED
{
    uint16_t vehicle_id;
    uint8_t  position;     /* 0..N-1 */
    uint8_t  orientation;  /* orn_orientation_t */
} orn_consist_entry_t;

/* ============================================================
 * TRAIN_COMMAND
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint8_t direction;   /* orn_direction_t in consist coordinates */
    uint8_t speed_cmd;   /* 0..255 */
    uint8_t brake_cmd;   /* 0..255 */
    uint8_t flags;       /* implementation-defined control flags */
} orn_payload_train_command_t;

/* ============================================================
 * EMERGENCY_STOP
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint8_t reason_code;
    uint8_t source_class;
} orn_payload_emergency_stop_t;

/* ============================================================
 * VEHICLE_STATUS
 * Generic status for any node
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint8_t  state_flags;
    uint8_t  battery_percent;
    int8_t   temperature_c;
    uint8_t  reserved0;
    uint16_t error_flags;
} orn_payload_vehicle_status_t;

/* ============================================================
 * LOCO_STATUS
 * Loco-specific runtime status
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint8_t  actual_direction; /* orn_direction_t or documented local mapping */
    uint8_t  actual_speed;
    uint8_t  traction_percent;
    uint8_t  brake_percent;
    uint16_t error_flags;
} orn_payload_loco_status_t;

/* ============================================================
 * ACK / NACK
 *
 * Shared payload for ORN_PKT_ACK and ORN_PKT_NACK. These frames should
 * carry ORN_FLAG_IS_ACK in header.verflg.
 * ============================================================ */

typedef struct ORN_PACKED
{
    uint8_t acked_type; /* orn_packet_type_t */
    uint8_t acked_seq;
    uint8_t code;       /* 0 for ACK, nonzero status/error for NACK */
    uint8_t reserved;
} orn_payload_ack_t;

/* ============================================================
 * Helper Macros / Functions
 * ============================================================ */

static inline void orn_frame_init_preamble(orn_frame_t *frame)
{
    frame->preamble.pre1 = ORN_PREAMBLE_1;
    frame->preamble.pre2 = ORN_PREAMBLE_2;
}

static inline void orn_header_init(orn_header_t *hdr,
                                   uint8_t type,
                                   uint8_t priority,
                                   uint16_t src,
                                   uint16_t dst,
                                   uint8_t seq,
                                   uint8_t flags,
                                   uint8_t payload_len)
{
    hdr->verflg   = ORN_VERFLG_MAKE(ORN_PROTO_VERSION_V0_1, flags);
    hdr->type     = type;
    hdr->priority = priority;
    hdr->src      = src;
    hdr->dst      = dst;
    hdr->seq      = seq;
    hdr->len      = payload_len;
}

static inline void orn_frame_init(orn_frame_t *frame,
                                  uint8_t type,
                                  uint8_t priority,
                                  uint16_t src,
                                  uint16_t dst,
                                  uint8_t seq,
                                  uint8_t flags,
                                  uint8_t payload_len)
{
    orn_frame_init_preamble(frame);
    orn_header_init(&frame->header, type, priority, src, dst, seq, flags, payload_len);
}

static inline bool orn_is_broadcast(uint16_t dst)
{
    return dst == ORN_NODE_ID_BROADCAST;
}

static inline uint16_t orn_frame_total_size(const orn_frame_t *frame)
{
    return (uint16_t)(
        sizeof(orn_preamble_t) +
        sizeof(orn_header_t) +
        frame->header.len +
        sizeof(uint16_t));
}

static inline uint16_t *orn_frame_crc_ptr(orn_frame_t *frame)
{
    return (uint16_t *)&frame->payload[frame->header.len];
}

static inline const uint16_t *orn_frame_crc_cptr(const orn_frame_t *frame)
{
    return (const uint16_t *)&frame->payload[frame->header.len];
}

/* Helpers for variable-length payload sizing */

static inline uint8_t orn_discover_auth_payload_len(uint8_t record_count)
{
    return (uint8_t)(sizeof(orn_payload_discover_auth_t) +
                     (record_count * sizeof(orn_discover_record_t)));
}

static inline uint8_t orn_consist_table_payload_len(uint8_t entry_count)
{
    return (uint8_t)(sizeof(orn_payload_consist_table_t) +
                     (entry_count * sizeof(orn_consist_entry_t)));
}

static inline orn_discover_record_t *orn_discover_auth_records(void *payload)
{
    return (orn_discover_record_t *)((uint8_t *)payload + sizeof(orn_payload_discover_auth_t));
}

static inline const orn_discover_record_t *orn_discover_auth_records_const(const void *payload)
{
    return (const orn_discover_record_t *)((const uint8_t *)payload + sizeof(orn_payload_discover_auth_t));
}

static inline orn_consist_entry_t *orn_consist_table_entries(void *payload)
{
    return (orn_consist_entry_t *)((uint8_t *)payload + sizeof(orn_payload_consist_table_t));
}

static inline const orn_consist_entry_t *orn_consist_table_entries_const(const void *payload)
{
    return (const orn_consist_entry_t *)((const uint8_t *)payload + sizeof(orn_payload_consist_table_t));
}

#ifndef __GNUC__
#pragma pack(pop)
#endif

#endif /* ORN_PROTO_H */
