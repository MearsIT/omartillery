#!/bin/bash
# Omartillery Plugin Installation Script
# This script installs the omartillery plugin to Omarchy

set -e

PLUGIN_NAME="omartillery"
SOURCE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PLUGINS_DIR="${HOME}/.config/omarchy/plugins"
INSTALL_DIR="${PLUGINS_DIR}/${PLUGIN_NAME}"
SHELL_CONFIG="${HOME}/.config/omarchy/shell.json"

echo "=== Omartillery Plugin Installer ==="
echo ""

# Step 1: Create plugins directory
echo "[1/4] Creating plugins directory..."
mkdir -p "${PLUGINS_DIR}"

# Step 2: Remove old installation if exists
if [ -d "${INSTALL_DIR}" ]; then
    echo "[2/4] Removing old installation..."
    rm -rf "${INSTALL_DIR}"
else
    echo "[2/4] No previous installation found"
fi

# Step 3: Copy plugin files
echo "[3/4] Installing plugin files..."
cp -r "${SOURCE_DIR}" "${INSTALL_DIR}"

# Remove development artifacts
rm -rf "${INSTALL_DIR}/.git" "${INSTALL_DIR}/.claude" "${INSTALL_DIR}/build" "${INSTALL_DIR}/Testing"

# Step 4: Configure shell (if needed)
echo "[4/4] Configuring shell..."
if [ -f "${SHELL_CONFIG}" ]; then
    # Check if already configured
    if grep -q "\"${PLUGIN_NAME}\"" "${SHELL_CONFIG}"; then
        echo "    Plugin already configured in shell.json"
    else
        echo "    Please manually add to ${SHELL_CONFIG}:"
        echo '    "plugins": {'
        echo '      "omartillery": {'
        echo '        "enabled": true'
        echo '      }'
        echo '    }'
    fi
else
    echo "    Creating new shell.json..."
    cat > "${SHELL_CONFIG}" <<EOF
{
  "plugins": {
    "${PLUGIN_NAME}": {
      "enabled": true
    }
  }
}
EOF
fi

echo ""
echo "=== Installation Complete! ==="
echo ""
echo "Next steps:"
echo "  1. Reload Omarchy plugins:"
echo "     omarchy-shell shell rescanPlugins"
echo ""
echo "  2. Launch the game:"
echo "     omarchy-shell shell summon omartillery '{}'"
echo ""
echo "  3. Hide the game:"
echo "     omarchy-shell shell hide omartillery"
echo ""
echo "Enjoy Artillery Duel!"
