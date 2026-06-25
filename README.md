# libsaguaro

**Saguaro** is a lightweight, authorization library written in pure C with no dependencies. This library has been built to be in-memory however it's been designed to have a simple integration with SQL environments (see below). The intention is to be as simple and concise as possible.

## Features

- Small API surface
- Uses application-defined identifiers (`uint64_t`)
- Supports many-to-many role assignments
- Supports many-to-many permission assignments
- Hierarchical role inheritance with cycle detection
- Duplicate relationship protection
- Fail-closed authorization model
- Easy integration with SQL backends
- Suitable for web services, CLIs, daemons, and embedded systems

The application is responsible for authentication and identity management.

## Design

The library stores only relationships. 

### Role Inheritance

This enables the creation of hierarchical authorization models without duplicating permission assignments.

Example:

```text
ROLE_ADMIN
    --> ROLE_EDITOR
            --> ROLE_USER
```

Permissions assigned to `ROLE_USER` are automatically available to `ROLE_EDITOR` and `ROLE_ADMIN`. Permissions assigned to `ROLE_EDITOR` are automatically available to `ROLE_ADMIN`.

```text
ROLE_USER
    --> PERM_READ

ROLE_EDITOR
    --> PERM_WRITE

ROLE_ADMIN
    --> PERM_DELETE
```

Results:

```text
ROLE_USER
    PERM_READ

ROLE_EDITOR
    PERM_READ
    PERM_WRITE

ROLE_ADMIN
    PERM_READ
    PERM_WRITE
    PERM_DELETE
```

Role inheritance relationships are validated when created and cyclical inheritance graphs are rejected.

Example:

```text
ADMIN -> EDITOR
EDITOR -> USER
USER -> ADMIN
```

The above configuration would create a cycle and will return `SAGUARO_ERR_CYCLE`.

## Build

To build a shared object for your system, run:

```sh
make
```

```sh
sudo make install
```

## Tests

All tests can be found in the `tests/` directory and are easily extended.

To run:

```sh
make tests
```

Tests are compiled with debug symbols.

## SQL Integration

The assumption is made that the application is responsible for the subjects, roles, and permissions, for now. The expansion of roles and permissions to be managed by `saguaro` might be added if proven necessary. Subjects will always, no matter what, be managed by the application.

Example schema:

```sql
CREATE TABLE subject_roles (
    subject_id BIGINT NOT NULL,
    role_id BIGINT NOT NULL,
    PRIMARY KEY (subject_id, role_id)
);

CREATE TABLE role_permissions (
    role_id BIGINT NOT NULL,
    permission_id BIGINT NOT NULL,
    PRIMARY KEY (role_id, permission_id)
);

CREATE TABLE role_inheritance (
    parent_role_id BIGINT NOT NULL,
    child_role_id BIGINT NOT NULL,
    PRIMARY KEY (parent_role_id, child_role_id)
);

-- additional indexes
CREATE INDEX idx_subject_roles_role
    ON subject_roles (role_id);

CREATE INDEX idx_role_permissions_permission
    ON role_permissions (permission_id);

CREATE INDEX idx_role_inheritance_child
    ON role_inheritance (child_role_id);
```

## Contributing

Please feel free to open a PR!

## Contact

Brian Downs [@bdowns328](http://twitter.com/bdowns328)

## License

BSD 2 Clause [License](/LICENSE).
