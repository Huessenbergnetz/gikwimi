let mix = require('laravel-mix');

mix.options({
    processCssUrls: false,
    terser: {
        terserOptions: {
            output: {
                comments: /@license/i
            }
        }
    }
});

mix.sass('assets/sass/style.scss', 'static/css/');

mix.scripts([
    'assets/js/licenseheader.js',
    'node_modules/@popperjs/core/dist/umd/popper.js',
    'node_modules/bootstrap/dist/js/bootstrap.js',
    'assets/js/general.js',
    'assets/js/guest.js',
    'assets/js/guestEdit.js',
    'assets/js/guestRemove.js',
    'assets/js/guestInvite.js',
    'assets/js/guestAdd.js',
    'assets/js/group.js',
    'assets/js/groupAdd.js'
], 'static/js/scripts.js');

mix.copy('node_modules/bootstrap-icons/font/fonts', 'static/fonts');
