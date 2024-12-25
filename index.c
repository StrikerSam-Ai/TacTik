// -------- preprocessor -----------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

// -------- terminal color code -----------
#define RESET "\x1b[0m"
#define CYAN "\x1b[36m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define RED "\x1b[31m"
#define BLUE "\x1b[34m"

// -------- inital values regarding the subjects and days -----------
#define MAX_DAYS 7
#define MAX_SUBJECTS 8
#define MAX_LINE_LENGTH 100

// -------- function declaration -----------
void print_welcome();
void display_loading_screen();
void animated_text(const char *text, int center);
void read_interactive(char timetable[MAX_DAYS][MAX_LINE_LENGTH], int *days);
void read_file_mode(char filename[], char timetable[MAX_DAYS][MAX_LINE_LENGTH],
                    int *days);
void prioritize_subjects(char timetable[MAX_DAYS][MAX_LINE_LENGTH], int days,
                         int tomorrow_index);

int count_occurrences(const char *line, const char *subject);
int get_terminal_width();
int get_tomorrow_index();
const char *welcome_quote();
void display_menu();
void about_us();
void exit_program();

// -------- main function -----------
int main() {
  char timetable[MAX_DAYS][MAX_LINE_LENGTH];
  int days = 0;
  int choice;

  print_welcome();

  while (1) {
    display_menu();

    printf(YELLOW "Your Choice: " RESET);
    scanf("%d", &choice);
    getchar();

    switch (choice) {
    case 1:
      read_interactive(timetable, &days);
      break;
    case 2: {
      char filename[50];
      printf(GREEN
             "Please enter the custom planner file name (with .txt): " RESET);
      fgets(filename, sizeof(filename), stdin);
      strtok(filename, "\n");
      read_file_mode(filename, timetable, &days);
    } break;
    case 3:
      about_us();
      break;
    case 4:
      exit_program();
      return 0;
    default:
      printf(RED "Invalid choice. Please try again." RESET "\n");
      break;
    }

    if (choice == 1 || choice == 2) {
      int tomorrow_index = get_tomorrow_index();
      if (tomorrow_index >= 0 && tomorrow_index < days) {
        prioritize_subjects(timetable, days, tomorrow_index);
      } else {
        printf(RED "Invalid day index. Returning to the menu." RESET "\n");
      }
    }
  }

  return 0;
}

// -------- finding the terminal width -- helps in centering the text
// -----------
int get_terminal_width() {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  int columns = 80; // Default width
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  }
  return columns;
#else
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  return w.ws_col;
#endif
}

// -------- centering the text/string that is given in the argument of the
// function -----------
void center_text(const char *text) {
  int width = get_terminal_width();
  int text_len = strlen(text);
  int padding = (width - text_len) / 2;
  for (int i = 0; i < padding; i++)
    printf(" ");
  printf("%s\n", text);
}

// -------- printing the logo -----------
void logo() {
  printf(YELLOW);
  center_text("      _---~~(~~-_.");
  center_text("    _{        )   )");
  center_text("  ,   ) -~~- ( ,-' )_");
  center_text(" (  `-,_..`., )-- '_,)");
  center_text("( ` _)  (  -~( -_ `,  }");
  center_text("(_-  _  ~_-~~~~`,  ,' )");
  center_text("  `~ -^(    __;-,((()))");
  center_text("        ~~~~ {_ -_(())");
  center_text("               `\  }");
  center_text("                 { }   ");
  printf(" ");
  printf(" ");
  center_text("A CAI - A Project made entirely with C");
  printf(RESET);
}

// -------- quote generator -----------
const char *welcome_quote() {

  const char *quotes[] = {
      "Success is the result of preparation, hard work, and learning from "
      "failure.",
      "An investment in knowledge always pays the best interest.",
      "Each new day is an opportunity to start fresh and achieve your goals.",
      "The more you learn, the more you earn and grow.",
      "By failing to prepare, you are preparing to fail.",
      "A goal without a plan is just a wish. Start planning your success "
      "today.",
      "Don't let yesterday take up too much of today.",
      "The best way to start the day is to plan for a productive tomorrow.",
      "Learn from yesterday, live for today, and prepare for tomorrow.",
      "Study hard. Know more today than you did yesterday, and strive for even "
      "more tomorrow.",
      "Mistakes are proof that you are trying. Learn from them and move "
      "forward.",
      "Dream big, plan well, and work harder than ever to achieve your goals.",
      "Success in learning comes to those who study persistently and plan "
      "wisely.",
      "Every morning you have two choices: continue to sleep with your dreams "
      "or wake up and chase them.",
      "Planning is bringing the future into the present, so you can take "
      "action now."};
  int num_quotes = sizeof(quotes) / sizeof(quotes[0]);

  srand(time(NULL));
  int random_index = rand() % num_quotes;

  return quotes[random_index];
}

// -------- welcome screen -----------
void print_welcome() {
  printf("\n ");
  printf("\n");
  display_loading_screen();
  printf("\n ");
  printf("\n");
  logo();
  printf("\n ");
  printf("\n");
  printf(CYAN);

  printf(" ");
  center_text("      * WELCOME TO TACTIK *     ");
  center_text("=============================================");

  printf("\n");
  center_text(
      "       TacTik is a thoughtfully designed tool "
      "crafted to help students take control of their study schedules. ");
  printf("\n ");
  center_text(" This innovative CLI-based program goes beyond just "
              "listing tomorrow's "
              "classes;");
  center_text("      it dynamically evaluates your upcoming timetable by "
              "analyzing your recent ");
  center_text("      study patterns, upcoming workload, and personal input "
              "about subject difficulty");
  printf("\n ");
  center_text("      The result? A customized list of prioritized "
              "subjects that ensures you're fully prepared and focused on what "
              "matters most.");

  printf("\n ");
  printf("\n");

  printf(GREEN);
  center_text("=============================================");
  const char *quote = welcome_quote();
  center_text(quote);
  center_text("=============================================");
  printf(RESET);
}

// -------- displaying the choices in the welcome screen -----------
void display_menu() {
  printf("\n");
  printf(GREEN "==================+=================" RESET);
  printf(GREEN "\n Please choose an option:\n" RESET);
  printf("1. Interactive Mode\n");
  printf("2. File Mode\n");
  printf("3. About Us\n");
  printf("4. Exit Program\n");
  printf(" ");
}

// -------- display the loading screen animation -----------
void display_loading_screen() {
  const char *loading_message = "Loading TacTik Application";
  const char *spinner = "|/-\\";
  int width = get_terminal_width();
  int bar_width = width - 20;

  printf(YELLOW "\n");
  center_text(loading_message);
  printf("\n");

  for (int i = 0; i <= bar_width; i += 2) {
    printf("\r[");
    for (int j = 0; j < i; j++) {
      printf("=");
    }
    for (int j = i; j < bar_width; j++) {
      printf(" ");
    }
    printf("] %c", spinner[i / 2 % 4]);
    fflush(stdout);
    usleep(20000);
  }
  printf("\n" RESET);
}

// -------- typing text animation -----------
void animated_text(const char *text, int center) {
  if (center) {
    int width = get_terminal_width();
    int text_len = strlen(text);
    int padding = (width - text_len) / 2;
    for (int i = 0; i < padding; i++)
      printf(" ");
  }
  while (*text) {
    putchar(*text++);
    fflush(stdout);
    usleep(50000);
  }
  printf("\n");
}

// -------- finding the tomorrow's index -----------
int get_tomorrow_index() {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int tomorrow_index =
      (tm.tm_wday + 1) % 7; // tm_wday: 0 (Sunday) to 6 (Saturday)
  return tomorrow_index >= 0 && tomorrow_index < MAX_DAYS ? tomorrow_index : -1;
}

// --------- interactive mode code -----------
void read_interactive(char timetable[MAX_DAYS][MAX_LINE_LENGTH], int *days) {
  printf(YELLOW "Enter the number of days (max %d): " RESET, MAX_DAYS);
  scanf("%d", days);
  getchar();

  const char *day_names[] = {"Monday", "Tuesday",  "Wednesday", "Thursday",
                             "Friday", "Saturday", "Sunday"};

  for (int i = 0; i < *days; i++) {
    printf(GREEN "Enter subjects for %s (comma-separated): " RESET,
           day_names[i]);
    fgets(timetable[i], MAX_LINE_LENGTH, stdin);
    strtok(timetable[i], "\n"); // Remove newline character
  }
}

// --------- file reading mode -----------
void read_file_mode(char filename[], char timetable[MAX_DAYS][MAX_LINE_LENGTH],
                    int *days) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf(RED "Error: Could not open file '%s'. Please ensure it exists and "
               "retry.\n" RESET,
           filename);
    return;
  }

  char line[MAX_LINE_LENGTH];
  *days = 0;

  while (fgets(line, sizeof(line), file)) {
    if (*days >= MAX_DAYS) {
      printf(YELLOW "Warning: More days in file than supported (%d). Trimming "
                    "extra days.\n" RESET,
             MAX_DAYS);
      break;
    }
    strtok(line, "\n"); // Remove newline character
    strncpy(timetable[*days], line, MAX_LINE_LENGTH - 1);
    timetable[*days][MAX_LINE_LENGTH - 1] = '\0'; // Ensure null termination
    (*days)++;
  }

  fclose(file);
}

// --------- prioritize subject algorithm - (today, yesterday) is compared with
// (tomorrow and day-after-tomorrow)-----------
void prioritize_subjects(char timetable[MAX_DAYS][MAX_LINE_LENGTH], int days,
                         int tomorrow_index) {
  char subjects[MAX_SUBJECTS][50];
  int forward_counts[MAX_SUBJECTS] = {0};
  int backward_counts[MAX_SUBJECTS] = {0};
  int difficulty[MAX_SUBJECTS];
  int scores[MAX_SUBJECTS];
  double normalized_scores[MAX_SUBJECTS];
  int subject_count = 0;

  // Forward counts for upcoming days
  for (int day_offset = 0; day_offset < 2; day_offset++) {
    int day_index = (tomorrow_index + day_offset) % days;
    char *token = strtok(timetable[day_index], ",");
    while (token != NULL) {

      while (isspace((unsigned char)*token))
        token++;
      char *end = token + strlen(token) - 1;
      while (end > token && isspace((unsigned char)*end))
        *end-- = '\0';

      int found = 0;
      for (int i = 0; i < subject_count; i++) {
        if (strcmp(subjects[i], token) == 0) {
          forward_counts[i]++;
          found = 1;
          break;
        }
      }
      if (!found) {
        strncpy(subjects[subject_count], token,
                sizeof(subjects[subject_count]));
        forward_counts[subject_count] = 1;
        subject_count++;
      }

      token = strtok(NULL, ",");
    }
  }

  // Backward counts for today and recent days
  for (int day_offset = 0; day_offset <= 2; day_offset++) {
    int day_index = (tomorrow_index - day_offset + days) % days;
    char *token = strtok(timetable[day_index], ", ");
    while (token != NULL) {
      for (int i = 0; i < subject_count; i++) {
        if (strcmp(subjects[i], token) == 0) {
          backward_counts[i]++;
          break;
        }
      }
      token = strtok(NULL, ", ");
    }
  }

  printf(GREEN
         "\nRate the difficulty of the subjects (1=Easy, 10=Hard):\n" RESET);
  for (int i = 0; i < subject_count; i++) {
    printf(YELLOW "%s: " RESET, subjects[i]);
    scanf("%d", &difficulty[i]);
    if (difficulty[i] < 1 || difficulty[i] > 10)
      difficulty[i] = 5;
  }

  // Calculate scores and normalize
  int max_score = 0;
  for (int i = 0; i < subject_count; i++) {
    scores[i] = forward_counts[i] * 2 - backward_counts[i] + difficulty[i];
    if (scores[i] > max_score)
      max_score = scores[i];
  }
  for (int i = 0; i < subject_count; i++) {
    normalized_scores[i] =
        (double)scores[i] / max_score * 100.0; // Convert to percentile
  }

  for (int i = 0; i < subject_count - 1; i++) {
    for (int j = i + 1; j < subject_count; j++) {
      if (normalized_scores[i] < normalized_scores[j]) {
        int temp_score = scores[i];
        scores[i] = scores[j];
        scores[j] = temp_score;

        double temp_percentile = normalized_scores[i];
        normalized_scores[i] = normalized_scores[j];
        normalized_scores[j] = temp_percentile;

        char temp_subject[50];
        strncpy(temp_subject, subjects[i], sizeof(temp_subject));
        strncpy(subjects[i], subjects[j], sizeof(subjects[i]));
        strncpy(subjects[j], temp_subject, sizeof(subjects[j]));

        int temp_count = forward_counts[i];
        forward_counts[i] = forward_counts[j];
        forward_counts[j] = temp_count;

        temp_count = backward_counts[i];
        backward_counts[i] = backward_counts[j];
        backward_counts[j] = temp_count;

        temp_count = difficulty[i];
        difficulty[i] = difficulty[j];
        difficulty[j] = temp_count;
      }
    }
  }

  animated_text(GREEN "\nFinal Priorities with Recommendations:\n" RESET, 1);
  for (int i = 0; i < subject_count; i++) {
    printf(YELLOW "#%d %s \t \t (Attention Required = %.2f%%): " RESET, i + 1,
           subjects[i], normalized_scores[i]);
    if (normalized_scores[i] > 75.0) {
      printf("Highly recommended! Focus intensively; important and "
             "challenging.\n");
    } else if (normalized_scores[i] > 50.0) {
      printf("Recommended to revise. A solid effort is needed.\n");
    } else if (normalized_scores[i] > 25.0) {
      printf("Moderate priority. A quick review will suffice.\n");
    } else {
      printf("Low priority. Minimal effort required; you're well-prepared.\n");
    }
  }
}

// --------- about us and documentation code -----------
void about_us() {
  printf("\n");

  printf(CYAN "What is TacTik?\n" RESET);
  printf(CYAN "===========================\n\n" RESET);
  printf("TacTik is a thoughtfully designed tool crafted to help "
         "students take control of their study schedules. ");
  printf(
      "This innovative CLI-based program goes beyond just listing tomorrow's "
      "classes; it dynamically evaluates your upcoming timetable ");
  printf("by analyzing your recent study patterns, upcoming workload, and "
         "personal input about subject difficulty. The result? ");
  printf("A customized list of prioritized subjects that ensures you're fully "
         "prepared and focused on what matters most.\n\n");
  printf("With a sleek ASCII-art UI, engaging animations, and intelligent "
         "algorithms working behind the scenes, ");
  printf("TacTik is more than just a tool - it's your smart study "
         "companion.\n\n");

  printf(CYAN "Who is it Useful For?\n" RESET);
  printf(CYAN "=====================\n\n" RESET);
  printf("TacTik is perfect for:\n");
  printf("- " GREEN "Students" RESET
         " who want to stay ahead in their academics and make strategic use of "
         "their time.\n");
  printf("- " GREEN "Planners" RESET
         " who love organization and seek to prioritize tasks based on their "
         "importance and urgency.\n");
  printf("- " GREEN "Procrastinators" RESET
         " who find it difficult to decide where to start preparing and need "
         "that extra push.\n");
  printf("- " GREEN "Teachers or Mentors" RESET
         ", who can use this tool to suggest better ways for their students to "
         "prepare for classes or exams.\n\n");
  printf("From high school to college, this application caters to students of "
         "all levels, helping them align their study habits with their "
         "academic goals.\n\n");

  printf(CYAN "The Uses of Spaced Repetition and SMART Goals\n" RESET);
  printf(CYAN "============================================\n\n" RESET);
  printf("- " YELLOW "Spaced Repetition:" RESET
         " TacTik integrates the principles of spaced repetition "
         "to evaluate subject focus. ");
  printf("It ensures subjects you haven't studied in recent days are given "
         "higher priority, creating natural and effective review intervals. ");
  printf("This makes retention and long-term learning far easier.\n\n");
  printf("- " YELLOW "SMART Goals:" RESET
         " The program aligns its recommendations with the principles of SMART "
         "(Specific, Measurable, Achievable, Relevant, Time-bound) goals. ");
  printf("By identifying clear priorities for the next day and weighing the "
         "difficulty levels, TacTik helps you achieve measurable ");
  printf("and realistic progress while maintaining a focused and relevant "
         "approach to your studies.\n\n");
  printf("Together, these techniques bring science-backed study strategies to "
         "your fingertips.\n\n");

  printf(CYAN "About the Team\n" RESET);
  printf(CYAN "==============\n\n" RESET);
  printf("TacTik was developed with passion by a team dedicated "
         "to blending technology and education. ");
  printf("Driven by the desire to make learning both effective and engaging, "
         "the creators come from diverse backgrounds, ");
  printf("bringing expertise in software development, education strategies, "
         "and UX design.\n\n");
  printf("We believe that small, intentional changes in how you approach "
         "learning can have a massive impact on your academic journey. ");
  printf("Our goal is to inspire students to take control of their preparation "
         "process, feel less overwhelmed, and ultimately excel in their "
         "pursuits.\n\n");
  printf("When we aren't busy improving TacTik, we're exploring "
         "ways to integrate modern AI and data-driven methodologies ");
  printf("to push the boundaries of educational tools further.\n\n");
  printf("Let TacTik simplify your study process so you can focus on "
         "success. Together, let's build a brighter and smarter tomorrow!\n\n");
}

// --------- exit the program code -----------
void exit_program() {
  printf(GREEN "\nThank you for using TacTik!\n" RESET);
  printf("We hope it helps you manage your studies effectively. Have a great "
         "day ahead!\n");
  sleep(5);
}
