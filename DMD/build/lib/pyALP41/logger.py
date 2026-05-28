import os
import logging


class LogHandler:
    def __init__(self, module_name: str, log_dir: str = None, log_level: str = "INFO"):
        self.logger = logging.getLogger(module_name)

        formatter = logging.Formatter(
            "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
        )

        log_filename = os.path.join(log_dir, f"pyALP41.log") if log_dir else "pyALP41.log"
        file_handler = logging.FileHandler(log_filename)
        file_handler.setFormatter(formatter)

        stream_handler = logging.StreamHandler()
        stream_handler.setFormatter(formatter)

        if not self.logger.handlers:
            self.logger.addHandler(file_handler)
            self.logger.addHandler(stream_handler)

        self.set_log_level(log_level)

    def error(self, exception: Exception):
        self.logger.error(f"Exception occurred: {exception}", exc_info=True)
        raise exception

    def info(self, detail: str = None):
        self.logger.info(detail)

    def set_log_level(self, level: str):
        levels = {
            "DEBUG": logging.DEBUG,
            "INFO": logging.INFO,
            "WARNING": logging.WARNING,
            "ERROR": logging.ERROR,
            "CRITICAL": logging.CRITICAL,
        }
        level = level.upper()
        if level in levels:
            self.logger.setLevel(levels[level])
            for handler in self.logger.handlers:
                handler.setLevel(levels[level])
        else:
            self.logger.warning(f"Invalid log level: {level}")
