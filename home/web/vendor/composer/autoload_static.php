<?php

// autoload_static.php @generated by Composer

namespace Composer\Autoload;

class ComposerStaticInit45d219aac092aa6e2b5b89113bb17341
{
    public static $prefixLengthsPsr4 = array (
        'T' => 
        array (
            'Twilio\\' => 7,
        ),
    );

    public static $prefixDirsPsr4 = array (
        'Twilio\\' => 
        array (
            0 => __DIR__ . '/..' . '/twilio/sdk/Twilio',
        ),
    );

    public static function getInitializer(ClassLoader $loader)
    {
        return \Closure::bind(function () use ($loader) {
            $loader->prefixLengthsPsr4 = ComposerStaticInit45d219aac092aa6e2b5b89113bb17341::$prefixLengthsPsr4;
            $loader->prefixDirsPsr4 = ComposerStaticInit45d219aac092aa6e2b5b89113bb17341::$prefixDirsPsr4;

        }, null, ClassLoader::class);
    }
}
