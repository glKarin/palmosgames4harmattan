
if [ -n "$PREENV_ROOT" ]; then
	export PREENV_ROOT
else
	# This needs to improve, but works well enough for debugging.
	# wrapper.sh will set a hardcoded /opt path.
	export PREENV_ROOT=$(pwd)
fi

if [ -n "$PATH" ]; then
	export PATH="$PREENV_ROOT/bin:$PATH"
else
	export PATH="$PREENV_ROOT/bin"
fi

if [ -n "$LD_LIBRARY_PATH" ]; then
	export LD_LIBRARY_PATH="$PREENV_ROOT/lib:$LD_LIBRARY_PATH"
else
	export LD_LIBRARY_PATH="$PREENV_ROOT/lib"
fi

if [ -n "$LD_PRELOAD" ]; then
	export LD_PRELOAD="$PREENV_ROOT/lib/libsdlpre.so:$LD_PRELOAD"
else
	export LD_PRELOAD="$PREENV_ROOT/lib/libsdlpre.so"
fi

